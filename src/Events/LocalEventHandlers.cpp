#include "LocalEventHandlers.h"

#include "Cache.h"

#include "Util/FileUtils.h"
#include "QsLog/QsLog.h"
#include "APIClient/FileUploader.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

namespace Drive
{

LocalEventHandlerBase::LocalEventHandlerBase(LocalFileEvent localEvent,
		QObject *parent)
	: EventHandlerBase(parent), localEvent(localEvent)
{
	QLOG_TRACE() << "Thread" << this
		<< "is staring processing local file event:";

	localEvent.logCompact();
}

LocalEventHandlerBase::~LocalEventHandlerBase()
{
	QLOG_TRACE() << "Thread" << this
		<< "has finished processing local file event:";

	localEvent.logCompact();
}

// ===========================================================================

LocalFileOrFolderAddedEventHandler::LocalFileOrFolderAddedEventHandler(
	LocalFileEvent localEvent, QObject *parent)
	: LocalEventHandlerBase(localEvent, parent)
	, m_parentId(0)
	, m_remotePath(Utils::toRemotePath(localEvent.localPath()))
{
}

void LocalFileOrFolderAddedEventHandler::run()
{
	if (localEvent.type != LocalFileEvent::Added
		&& localEvent.type != LocalFileEvent::Modified)
	{
		return;
	}

	if (!QFile::exists(localEvent.localPath()))
	{
		return;
	}

	// TODO: don't process hidden files

	// Cached:

	LocalCache& localCache = LocalCache::instance();

	const RemoteFileDesc fd = localCache.file(m_remotePath);
	if (!fd.isValid())
	{
		m_parentId = localCache.file(m_remotePath, true).id;
		Q_ASSERT(m_parentId != 0);

		onGetFileObjectParentIdSucceeded(m_parentId);
		exec();
		return;
	}
	else
	{
		if (fd.type == RemoteFileDesc::Dir)
		{
			processEventsAndQuit();
			return;
		}
		else
		{
			onGetFileObjectSucceeded(fd);
			exec();
			return;
		}
	}
}

void LocalFileOrFolderAddedEventHandler::onGetFileObjectIdSucceeded(int id)
{
	QLOG_TRACE() << this << "getFileObjectIdSucceeded:" << id;

	// file object exists, let's get it and check its timestamp

	FilesRestResourceRef filesRestResource = FilesRestResource::create();

	connect(filesRestResource.data()
		, SIGNAL(getFileObjectSucceeded(Drive::RemoteFileDesc))
		, this
		, SLOT(onGetFileObjectSucceeded(Drive::RemoteFileDesc)));

	connect(filesRestResource.data(), SIGNAL(failed(QString)),
		this, SLOT(onGetFileObjectFailed(QString)));

	filesRestResource->getFileObject(id);
}

void LocalFileOrFolderAddedEventHandler::onGetFileObjectIdFailed()
{
	// no file object exists, let's create one

	QLOG_TRACE() << this << "getFileObjectIdFailed, let's create one.";

//	GetChildrenResourceRef getChildrenResource =
//		GetChildrenResource::create();

	m_getChildrenResource =
		GetChildrenResource::create();

	connect(m_getChildrenResource.data()
		, SIGNAL(getFileObjectIdSucceeded(int))
		, this
		, SLOT(onGetFileObjectParentIdSucceeded(int)));

	connect(m_getChildrenResource.data()
		, SIGNAL(getFileObjectIdFailed())
		, this
		, SLOT(onGetFileObjectParentIdFailed()));

	m_getChildrenResource->getFileObjectParentId(m_remotePath);
}

void LocalFileOrFolderAddedEventHandler::onGetFileObjectSucceeded(
	RemoteFileDesc fileDesc)
{
	// file object exists
	// if it's a folder, then do nothing and quit
	// if it's a file, then check timestamp
	//	if remote's one is newer, then do nothing and quit
	//	else delete and upload

	m_remoteFileDesc = fileDesc;

	QLOG_TRACE() << this << "getFileObjectSucceeded:" << fileDesc.id << fileDesc.name << fileDesc.modifiedAt;

	QFileInfo fileInfo(localEvent.localPath());
	if (fileInfo.isDir())
	{
		QLOG_INFO() << "Remote folder already exists: "
			<< m_remotePath;

		processEventsAndQuit();
	}
	else
	{
		if (fileDesc.modifiedAt >= fileInfo.lastModified().toTime_t())
		{
			QLOG_INFO() << this
				<< "Remote file object is not older than local, no need to update it."
				<< m_remotePath;
			QLOG_TRACE() << "remote ts:"
				<< QDateTime::fromTime_t(fileDesc.modifiedAt)
				<< "local ts:"
				<< QDateTime::fromTime_t(fileInfo.lastModified().toTime_t());

			processEventsAndQuit();
		}
		else
		{
			// trash, then delete

			TrashRestResourceRef trashRes = TrashRestResource::create();

			connect(trashRes.data(), SIGNAL(succeeded()),
				this, SLOT(onTrashSucceeded()));

			connect(trashRes.data(), SIGNAL(failed(QString)),
				this, SLOT(onTrashFailed(QString)));

			trashRes->trash(fileDesc.id);
		}
	}
}

void LocalFileOrFolderAddedEventHandler::onGetFileObjectFailed(
	const QString& error)
{
	QLOG_TRACE() << this << "getFileObjectFailed:" << error;

	processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler::
	onGetFileObjectParentIdSucceeded(int id)
{
	const QFileInfo fileInfo(localEvent.localPath());
	if (fileInfo.isDir())
	{
		auto createRestResource = CreateRestResource::create();

		connect(createRestResource.data(), &CreateRestResource::succeeded,
				this, &LocalFileOrFolderAddedEventHandler::onCreateFolderSucceeded);

		connect(createRestResource.data(), &CreateRestResource::failed,
				this, &LocalFileOrFolderAddedEventHandler::onCreateFolderFailed);

		createRestResource->createFolder(id, localEvent.fileName());
	}
	else
	{
		FileUploader *uploader = new FileUploader(this);

		connect(uploader, SIGNAL(succeeded(Drive::RemoteFileDesc)),
			this, SLOT(onUploadSucceeded(Drive::RemoteFileDesc)));

		connect(uploader, SIGNAL(failed(QString)),
			this, SLOT(onUploadFailed(QString)));

		uploader->uploadFile(id, localEvent.localPath());
	}
}

void LocalFileOrFolderAddedEventHandler::onGetFileObjectParentIdFailed()
{
	QLOG_ERROR() << this
		<< ": failed to create remote file object, no parent id";
	processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler
	::onCreateFolderSucceeded(Drive::RemoteFileDesc fileDesc)
{
	QLOG_TRACE() << "Remote folder created" << fileDesc.id << fileDesc.name;

	LocalCache::instance().addFile(fileDesc);

	RemoteFileEventExclusion
		exclusion(RemoteFileEvent::Created, fileDesc.id);
	emit newRemoteFileEventExclusion(exclusion);

	processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler::onCreateFolderFailed(
	const QString& error)
{
	QLOG_TRACE() << "Failed to create a remote folder";
	emit failed(error);
	processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler::
	onUploadSucceeded(Drive::RemoteFileDesc fileDesc)
{
	QLOG_TRACE() << this << "File uploaded:" << m_remotePath;

	if (fileDesc.isValid())
	{
		LocalCache::instance().addFile(fileDesc);

		RemoteFileEventExclusion exclusion(RemoteFileEvent::Uploaded, fileDesc.id);
		emit newRemoteFileEventExclusion(exclusion);
	}

	processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler::onUploadFailed(const QString& error)
{
	QLOG_ERROR() << this << "Failed to upload file:" << m_remotePath << error;
	emit failed(error);
	processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler::onTrashSucceeded()
{
	QLOG_TRACE() << this << "trash succeeded for id" << m_remoteFileDesc.id;

	FilesRestResourceRef filesRestResource = FilesRestResource::create();

	connect(filesRestResource.data(), SIGNAL(succeeded()),
		this, SLOT(onRemoveSucceeded()));

	connect(filesRestResource.data(), SIGNAL(failed(QString)),
		this, SLOT(onRemoveFailed(QString)));

	filesRestResource->remove(m_remoteFileDesc.id);

	RemoteFileEventExclusion
		exclusion(RemoteFileEvent::Trashed, m_remoteFileDesc.id);
	emit newRemoteFileEventExclusion(exclusion);
}

void LocalFileOrFolderAddedEventHandler::onTrashFailed(const QString& error)
{
	QLOG_ERROR() << this << ": failed to trash remote file object:" << m_remoteFileDesc.id;
	processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler::onRemoveSucceeded()
{
	QLOG_TRACE() << this << "remove succeeded:" << m_remoteFileDesc.id;
	onGetFileObjectIdFailed();
}

void LocalFileOrFolderAddedEventHandler::onRemoveFailed(const QString& error)
{
	QLOG_ERROR() << this << ": failed to remove remote file object:" << m_remoteFileDesc.id;
	processEventsAndQuit();
}

// ===========================================================================

LocalFileOrFolderDeletedEventHandler::LocalFileOrFolderDeletedEventHandler(
	LocalFileEvent localEvent, QObject *parent)
	: LocalEventHandlerBase(localEvent, parent)
	, m_remoteFileObjectId(0)
{
}

void LocalFileOrFolderDeletedEventHandler::run()
{
	if (localEvent.type != LocalFileEvent::Deleted)
		return;

	QString remotePath = Utils::toRemotePath(localEvent.localPath());

	// Cached:

	int objId = LocalCache::instance().file(remotePath).id;

	if (objId)
	{
		onGetFileObjectIdSucceeded(objId);
		exec();
		return;
	}

	// Not cached:

	GetChildrenResourceRef getChildrenResource = GetChildrenResource::create();

	connect(getChildrenResource.data(), &GetChildrenResource::getFileObjectIdSucceeded,
		this, &LocalFileOrFolderDeletedEventHandler::onGetFileObjectIdSucceeded);

	connect(getChildrenResource.data(), &GetChildrenResource::getFileObjectIdFailed,
		this, &LocalFileOrFolderDeletedEventHandler::onGetFileObjectIdFailed);

	getChildrenResource->getFileObjectId(remotePath);

	m_currentResource = getChildrenResource;

	exec();
}

void LocalFileOrFolderDeletedEventHandler::onGetFileObjectIdSucceeded(int id)
{
	QLOG_TRACE() <<
		"LocalFileOrFolderDeletedEventHandler file object id:" << id;

	TrashRestResourceRef trashRestResource = TrashRestResource::create();

	connect(trashRestResource.data(), &TrashRestResource::succeeded,
		this, &LocalFileOrFolderDeletedEventHandler::onTrashSucceeded);

	connect(trashRestResource.data(), &TrashRestResource::failed,
		this, &LocalFileOrFolderDeletedEventHandler::onTrashFailed);

	trashRestResource->trash(id);

	m_remoteFileObjectId = id;
	m_currentResource = trashRestResource;
}

void LocalFileOrFolderDeletedEventHandler::onGetFileObjectIdFailed()
{
	QLOG_ERROR() << "Failed to trash remote file object: no id";
	processEventsAndQuit();
}

void LocalFileOrFolderDeletedEventHandler::onTrashSucceeded()
{
	QLOG_TRACE() << "Remote filed object trashed.";

	if (m_remoteFileObjectId)
	{
		RemoteFileEventExclusion exclusion(RemoteFileEvent::Trashed
			, m_remoteFileObjectId);
		emit newRemoteFileEventExclusion(exclusion);
	}

	emit succeeded();
	processEventsAndQuit();
}

void LocalFileOrFolderDeletedEventHandler::onTrashFailed(const QString& error)
{
	QLOG_TRACE() << "Failed to trash a remote file object";
	emit failed(error);
	processEventsAndQuit();
}

// ===========================================================================

LocalFileOrFolderRenamedEventHandler::LocalFileOrFolderRenamedEventHandler(
	LocalFileEvent localEvent, QObject *parent)
	: LocalEventHandlerBase(localEvent, parent)
	, remoteFileObjectId(0)
{
}

void LocalFileOrFolderRenamedEventHandler::run()
{
	if (localEvent.type != LocalFileEvent::Moved)
		return;

	QFileInfo oldFileInfo(localEvent.oldLocalPath());
	QFileInfo newFileInfo(localEvent.localPath());

	if (oldFileInfo.dir().absolutePath() != newFileInfo.dir().absolutePath())
	{
		QLOG_TRACE() << "Local Move event: folders are not equal";
		return;
	}

	newName = newFileInfo.fileName();

	QString oldRemotePath =
		Utils::toRemotePath(localEvent.oldLocalPath());

		//	QString localFolderAbsolutePath = newFileInfo.dir().absolutePath();
		//	QString folderRemotePath =
		//		Utils::localPathToRemotePath(localFolderAbsolutePath);

	getChildrenResource = GetChildrenResource::create();

	connect(getChildrenResource.data(), SIGNAL(getFileObjectIdSucceeded(int)),
		this, SLOT(onGetFileObjectIdSucceeded(int)));

	connect(getChildrenResource.data(), SIGNAL(getFileObjectIdFailed()),
		this, SLOT(onGetFileObjectIdFailed()));

	getChildrenResource->getFileObjectId(oldRemotePath);

	exec();
}

void LocalFileOrFolderRenamedEventHandler::onGetFileObjectIdSucceeded(int id)
{
	remoteFileObjectId = id;

	QLOG_TRACE() <<
		"LocalFileOrFolderAddedEventHandler file object id:" << id;

	FilesRestResourceRef filesRestResource = FilesRestResource::create();

	connect(filesRestResource.data(), SIGNAL(succeeded()),
		this, SLOT(onRenameSucceeded()));

	connect(filesRestResource.data(), SIGNAL(failed(QString)),
		this, SLOT(onRenameFailed(QString)));

	filesRestResource->rename(id, newName);
}

void LocalFileOrFolderRenamedEventHandler::onGetFileObjectIdFailed()
{
	// if failed to find the remote file object,
	// then create new local event "added"
	QLOG_TRACE() << "Failed to rename remote file object: no id. "
		<< "Converting the Moved event to Added event";

	LocalFileEvent event(localEvent);
	event.type = LocalFileEvent::Added;
	emit newLocalFileEvent(event);

	processEventsAndQuit();
}

void LocalFileOrFolderRenamedEventHandler::onRenameSucceeded()
{
	QLOG_TRACE() << "Remote file object rename succeeded: "
		<< remoteFileObjectId << newName;

	if (remoteFileObjectId)
	{
		RemoteFileEventExclusion exclusion(RemoteFileEvent::Renamed
			, remoteFileObjectId);
		emit newRemoteFileEventExclusion(exclusion);
	}

	processEventsAndQuit();
}

void LocalFileOrFolderRenamedEventHandler::onRenameFailed(const QString& error)
{
	QLOG_ERROR() << "Failed to rename remote file object: "
		<< remoteFileObjectId << newName << error;

	processEventsAndQuit();
}

}
