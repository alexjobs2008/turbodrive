#include "Cache.h"

#include "QsLog/QsLog.h"

#include <QtCore/QMutexLocker>

#define ERRLOG QLOG_ERROR() << this << ": "
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
	m_idMap.clear();
	m_pathMap.clear();
}

int LocalCache::id(const QString& remotePath, const bool forParent) const
{
	LOCK_MUTEX;

	QString path = remotePath;

	if (forParent)
	{
		QStringList list = path.split("/", QString::SkipEmptyParts);
		list.removeLast();
		path = list.join("/");
	}

	if (m_pathMap.contains(path))
	{
		return m_pathMap.value(path).id;
	}
	else
	{
		ERRLOG << "Remote file descriptor not found "
			<< "(remote path: '" << remotePath << "').";
		return 0;
	}
}

RemoteFileDesc LocalCache::fileDescriptor(const QString& remotePath, const bool forParent) const
{
	LOCK_MUTEX;

	QString path = remotePath;

	if (forParent)
	{
		QStringList list = path.split("/", QString::SkipEmptyParts);
		list.removeLast();
		path = list.join("/");
	}

	if (m_pathMap.contains(path))
	{
		return m_pathMap.value(path);
	}
	else
	{
		ERRLOG << "Remote file descriptor not found "
			<< "(remote path: '" << remotePath << "').";

		RemoteFileDesc invalidDesc;
		invalidDesc.id = 0;
		invalidDesc.parentId = 0;
		invalidDesc.name = QString();
		return invalidDesc;
	}
}

void LocalCache::addRoot(const RemoteFileDesc& d)
{
	LOCK_MUTEX;

	if (d.type != RemoteFileDesc::Dir)
	{
		ERRLOG << "Root descriptor ignored (not a dir): " << d.toString() << ".";
		return;
	}

	if (isRootId(d.id))
	{
		ERRLOG << "Root descriptor ignored (already registered): " << d.toString() << ".";
		return;
	}

	m_rootIds.push_back(d.id);

	RemoteFileDesc rootDescriptor;
	rootDescriptor.id = d.id;
	rootDescriptor.type = RemoteFileDesc::Dir;
	rootDescriptor.parentId = -1;
	rootDescriptor.name = "#root/" + d.name;

	m_idMap.insert(rootDescriptor.id, rootDescriptor);
	m_pathMap.insert(rootDescriptor.name, rootDescriptor);
}

void LocalCache::addFile(const RemoteFileDesc& fileDescriptors)
{
	LOCK_MUTEX;

	if (m_rootIds.empty())
	{
		ERRLOG << "No root ids found.";
		return;
	}

	m_idMap.insert(fileDescriptors.id, fileDescriptors);
	insertIntoPathMap(fileDescriptors);
}

QString LocalCache::toString() const
{
	QByteArray result;
	QTextStream(&result)
		<< QLatin1String("[LocalCache:") << this << QLatin1String("]");
	return result;
}

void LocalCache::insertIntoPathMap(const RemoteFileDesc& fileDesc)
{
	QString path = fileDesc.name;
	if (fileDesc.type == RemoteFileDesc::Dir)
	{
		path.append("/");
	}

	RemoteFileDesc currentDesc = fileDesc;
	while (isRootId(currentDesc.id))
	{
		if (!m_idMap.contains(currentDesc.parentId))
		{
			ERRLOG
				<< "Cannot find parent path for remote file descriptor"
				<< " (currentDesc: " << currentDesc.toString()
				<<", fileDesc: " << fileDesc.toString() << ").";
			return;
		}

		currentDesc = m_idMap.value(currentDesc.parentId);
		path.prepend(currentDesc.name + "/");
	}

	m_pathMap.insert(path, fileDesc);
}

bool LocalCache::isRootId(const int id) const
{
	return find(m_rootIds.begin(), m_rootIds.end(), id) != m_rootIds.end();
}

}
