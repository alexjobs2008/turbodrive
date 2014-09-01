#include "Cache.h"
#include "Util/FileUtils.h"
#include "QsLog/QsLog.h"

#include <QtCore/QMutexLocker>

#define DLOG QLOG_DEBUG() << this->toString() << ": "
#define ERRLOG QLOG_ERROR() << this->toString() << ": "
#define LOCK_MUTEX QMutexLocker __mutexLocker(&m_mutex)

namespace Drive
{

LocalCache& LocalCache::instance()
{
	static LocalCache myself;
	return myself;
}

void LocalCache::clear()
{
	LOCK_MUTEX;
	m_files.clear();
}

RemoteFileDesc LocalCache::file(const QString& remotePath, const bool forParent) const
{
	const QString path = forParent ? Utils::parentPath(remotePath) : remotePath;

	{
		LOCK_MUTEX;
		auto it = m_files.find(path);
		if (it != m_files.end())
		{
			return it->second;
		}
	}

	RemoteFileDesc invalidDesc;
	invalidDesc.id = 0;
	invalidDesc.parentId = 0;
	invalidDesc.name = QString::null;
	return invalidDesc;
}

RemoteFileDesc LocalCache::file(const int id, const bool forParent) const
{
	const RemoteFileDesc result = fileById(id);
	if (!result.isValid())
	{
		ERRLOG << "Remote file descriptor not found "
			<< "(id: " << id << ").";
		return result;
	}

	return forParent ? file(result.parentId) : result;
}

void LocalCache::addRoot(const RemoteFileDesc& file)
{
	if (file.type != RemoteFileDesc::Dir)
	{
		ERRLOG << "Root descriptor ignored (not a dir): "
				<< file.toString() << ".";
		return;
	}

	if (fileById(file.id).isValid())
	{
		ERRLOG << "Root descriptor ignored (root with same id already exists): "
				<< file.toString() << ".";
		return;
	}

	RemoteFileDesc rootDescriptor(file);
	rootDescriptor.parentId = -1;
	addFile(rootDescriptor);
}

bool LocalCache::addFile(const RemoteFileDesc& file)
{
	removeById(file.id);
	const QString path = fullPath(file);

    if (!path.isNull())
	{
        {
            LOCK_MUTEX;
            m_files[path] = file;
        }
        return true;
	}
    else
    {
        QLOG_ERROR() << "LocalCache::addFile() did not add file " << file.name
                       << " (LocalCache::fullPath() returned null path string)";
        return false;
    }
}

void LocalCache::removeFile(const RemoteFileDesc& file)
{
    // Q_ASSERT(file.parentId != -1); ?
	Q_ASSERT(removeById(file.id));
}

QString LocalCache::fullPath(const RemoteFileDesc& d) const
{
    QString result = QString::null;
	RemoteFileDesc currentFile = d;
	for (;;)
	{
		result.prepend(currentFile.name);
		result.prepend(QLatin1String("/"));
		if(currentFile.parentId == -1)
		{
			result.prepend(QLatin1String("#root"));
			break;
		}
		currentFile = fileById(currentFile.parentId);

        if (currentFile.id == 0 && currentFile.parentId ==0 &&
                currentFile.name == QString::null)
        {
            QLOG_ERROR() << "LocalCache::fullPath() did not find parent file for path "
                       << result;
            return QString::null;
        }
	}
	return result;
}

RemoteFileDesc LocalCache::fileById(const int id) const
{
	{
		LOCK_MUTEX;

        /* for (auto it = m_files.begin(); it != m_files.end(); ++it)
		{
			if (it->second.id == id)
			{
				return it->second;
			}
        } */

		std::map<QString, RemoteFileDesc>::const_iterator end = m_files.cend();
        for (std::map<QString, RemoteFileDesc>::const_iterator it = m_files.cbegin();
             it != end;
             it++)
        {
            if (it->second.id == id)
            {
                return it->second;
            }
        }
	}

	RemoteFileDesc invalidDesc;
	invalidDesc.id = 0;
	invalidDesc.parentId = 0;
	invalidDesc.name = QString::null;
	return invalidDesc;
}

RemoteFileDesc LocalCache::fileByParentId(const int id) const
{
	{
		LOCK_MUTEX;
		for (auto it = m_files.begin(); it != m_files.end(); ++it)
		{
			if (it->second.parentId == id)
			{
				return it->second;
			}
		}
	}

	RemoteFileDesc invalidDesc;
	invalidDesc.id = 0;
	invalidDesc.parentId = 0;
	invalidDesc.name = QString::null;
	return invalidDesc;
}

bool LocalCache::removeById(const int id)
{
	return removeByIdImpl(id);
}

bool LocalCache::removeByIdImpl(int id)
{
	auto result = false;

	for (;;)
	{
		const auto desc = fileByParentId(id);
		if (!desc.isValid())
		{
			break;
		}
		Q_ASSERT(removeByIdImpl(desc.id));
	}

	{
		LOCK_MUTEX;
		for (auto it = m_files.begin(); it != m_files.end(); ++it)
		{
			if (it->second.id == id)
			{
				m_files.erase(it);
				result = true;
				break;
			}
		}
	}
	return result;
}

QString LocalCache::toString() const
{
	QString result;
	QTextStream(&result)
		<< QLatin1String("[LocalCache:") << this << QLatin1String("]");
	return result;
}

}
