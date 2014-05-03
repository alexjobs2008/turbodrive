#include "Cache.h"

#include "QsLog/QsLog.h"

#include <QtCore/QMutexLocker>

#define DLOG QLOG_DEBUG() << this->toString() << ": "
#define ERRLOG QLOG_ERROR() << this->toString() << ": "
#define LOCK_MUTEX QMutexLocker __mutexLocker(&m_mutex)

namespace Drive
{

namespace
{

QString parentPath(const QString& path)
{
	QStringList list = path.split("/", QString::SkipEmptyParts);
	list.removeLast();
	return list.join("/");
}

}

LocalCache& LocalCache::instance()
{
	static LocalCache myself;
	return myself;
}

void LocalCache::clear()
{
	m_files.clear();
}

RemoteFileDesc LocalCache::file(const QString& remotePath, const bool forParent) const
{
	LOCK_MUTEX;

	const QString path = forParent ? parentPath(remotePath) : remotePath;
	auto it = m_files.find(path);
	if (it != m_files.end())
	{
		return it->second;
	}

	ERRLOG << "Remote file descriptor not found "
		<< "(remote path: " << remotePath << ").";

	RemoteFileDesc invalidDesc;
	invalidDesc.id = 0;
	invalidDesc.parentId = 0;
	invalidDesc.name = QString::null;
	return invalidDesc;
}

RemoteFileDesc LocalCache::file(const int id, const bool forParent) const
{
	LOCK_MUTEX;

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
	// not need LOCK_MUTEX here, because we use addFile internally

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

void LocalCache::addFile(const RemoteFileDesc& file)
{
	LOCK_MUTEX;
	removeById(file.id);
	const QString path = fullPath(file);
	m_files.insert(std::make_pair(path, file));
}

QString LocalCache::fullPath(const RemoteFileDesc& d) const
{
	QString result;
	RemoteFileDesc currentFile = d;
	for (;;)
	{
		result.prepend(currentFile.name);
		result.prepend(QLatin1String("/"));
		if(currentFile.parentId == -1)
		{
			result.prepend(QLatin1String("/#root"));
			break;
		}
		currentFile = fileById(currentFile.parentId);
	}
	return result;
}

RemoteFileDesc LocalCache::fileById(const int id) const
{
	for (auto it = m_files.begin(); it != m_files.end(); ++it)
	{
		if (it->second.id == id)
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

bool LocalCache::removeById(const int id)
{
	auto result = false;
	for (auto it = m_files.begin(); it != m_files.end(); ++it)
	{
		if (it->second.id == id)
		{
			m_files.erase(it);
			result = true;
			break;
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
