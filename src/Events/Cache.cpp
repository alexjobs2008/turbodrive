#include "Cache.h"

#include "QsLog/QsLog.h"

#include <QtCore/QMutexLocker>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

namespace Drive
{

LocalCache& LocalCache::instance()
{
	static LocalCache myself;
	return myself;
}

LocalCache::LocalCache(QObject *parent)
	: QObject(parent)
	, diskId(0)
{
}

void LocalCache::clear()
{
	QMutexLocker locker(&mutex);
	idMap.clear();
	pathMap.clear();
}

void LocalCache::log(const QString& fileName)
{
	QString fname = fileName;
	if (fname.isEmpty())
	{
		QDir(QCoreApplication::applicationDirPath()).filePath("cacheLog.txt");
	}

	QFile logFile;
	logFile.setFileName(
		QDir(QCoreApplication::applicationDirPath()).filePath("eventLog.txt"));

	logFile.remove();
	logFile.open(QIODevice::WriteOnly | QIODevice::Text);

	QMapIterator<QString, RemoteFileDesc> i(pathMap);
	while (i.hasNext())
	{
		i.next();
		QString line = i.key();
		line.append(": ").append(i.value().id).append("\n");
		logFile.write(line.toUtf8());
	}

	logFile.flush();
	logFile.close();
}

int LocalCache::id(const QString& remotePath, bool forParent)
{
	QMutexLocker locker(&mutex);

	QString path = remotePath;

	if (forParent)
	{
		QStringList list = path.split("/", QString::SkipEmptyParts);
		list.removeLast();
		path = list.join("/");

		QLOG_TRACE() << "Local cache: checking id for " << path;
	}

	if (pathMap.contains(path))
	{
		QLOG_TRACE() << "Local cache: found: " << pathMap.value(path).id;
		return pathMap.value(path).id;
	}
	else
	{
		QLOG_ERROR() << "LocalCache: RemoteFileDesc not found for path "
			<< remotePath;

		return 0;
	}
}

RemoteFileDesc LocalCache::fileDesc(const QString& remotePath, bool forParent)
{
	QMutexLocker locker(&mutex);

	QString path = remotePath;

	if (forParent)
	{
		QStringList list = path.split("/", QString::SkipEmptyParts);
		list.removeLast();
		path = list.join("/");
	}

//	if (!path.startsWith("/"))
//	{
//		path.prepend("/");
//	}

	if (pathMap.contains(path))
	{
		return pathMap.value(path);
	}
	else
	{
		QLOG_ERROR() << "LocalCache: RemoteFileDesc not found for path "
			<< remotePath;

		RemoteFileDesc invalidDesc;
		invalidDesc.id = 0;
		invalidDesc.parentId = 0;
		invalidDesc.name = QString();
		return invalidDesc;
	}
}

void LocalCache::onRootId(int id)
{
	QMutexLocker locker(&mutex);

	if (diskId != 0)
	{
		QLOG_ERROR() << "LocalCache: disk duplicate id detected";
		return;
	}
	diskId = id;
	RemoteFileDesc diskDesc;
	diskDesc.id = diskId;
	diskDesc.type = RemoteFileDesc::Folder;
	diskDesc.parentId = -1;
	diskDesc.name = "#root/#disk";

	idMap.insert(diskDesc.id, diskDesc);
	pathMap.insert(diskDesc.name, diskDesc);

	QLOG_TRACE() << "LocalCache: disk id: " << diskId;
}

void LocalCache::onNewFileDesc(const RemoteFileDesc& fileDesc)
{
	QMutexLocker locker(&mutex);

	if (diskId == 0)
	{
		QLOG_ERROR() << "LocalCache: disk id not found";
		return;
	}

	idMap.insert(fileDesc.id, fileDesc);
	insertIntoPathMap(fileDesc);
}

void LocalCache::insertIntoPathMap(const RemoteFileDesc& fileDesc)
{
	QString path = fileDesc.name;
	if (fileDesc.type == RemoteFileDesc::Folder)
	{
		path.append("/");
	}

	RemoteFileDesc currentDesc = fileDesc;
	while (currentDesc.id != diskId)
	{
		if (!idMap.contains(currentDesc.parentId))
		{
			QLOG_ERROR()
					<< "LocalCache: cannot find parent path for RemoteFileDesc"
					<< " (currentDesc: " << currentDesc.toString()
					<<", fileDesc: " << fileDesc.toString() << ").";
			return;
		}

		currentDesc = idMap.value(currentDesc.parentId);
		path.prepend(currentDesc.name + "/");
	}

	pathMap.insert(path, fileDesc);
}

}
