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

void LocalCache::addRoot(const RemoteFileDesc& d)
{
	if (d.type != RemoteFileDesc::Dir)
	{
		ERRLOG << "Descriptor ignored (root and not a dir): " << d.toString() << ".";
		return;
	}

	RemoteFileDesc rootDescriptor(d);
	rootDescriptor.parentId = -1;
	addFile(rootDescriptor);
}

void LocalCache::addFile(const RemoteFileDesc& file)
{
	LOCK_MUTEX;
	const QString path = fullPath(file);
	DLOG << "New file: [" << path << ", " << file.toString() << ".";
	m_files.insert(std::make_pair(path, file));
}

QString LocalCache::fullPath(const RemoteFileDesc& file) const
{
	QString result;
	RemoteFileDesc currentFile = file;
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

QString LocalCache::toString() const
{
	QString result;
	QTextStream(&result)
		<< QLatin1String("[LocalCache:") << this << QLatin1String("]");
	return result;
}

}
