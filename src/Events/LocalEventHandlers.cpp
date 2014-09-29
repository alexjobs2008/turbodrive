#include "LocalEventHandlers.h"

#include "Application/factoriesstorage.h"
#include "Cache.h"

#include "Util/FileUtils.h"
#include "QsLog/QsLog.h"
#include "APIClient/FileUploader.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <list>

using namespace std;

namespace Drive
{

LocalEventHandlerBase::LocalEventHandlerBase(LocalFileEvent localEvent,
		QObject *parent)
	: EventHandlerBase(parent), localEvent(localEvent)
{
}

LocalEventHandlerBase::~LocalEventHandlerBase()
{
}

// ===========================================================================

LocalFileOrFolderAddedEventHandler::LocalFileOrFolderAddedEventHandler(
	LocalFileEvent localEvent, QObject *parent)
	: LocalEventHandlerBase(localEvent, parent)
	, m_parentId(0)
	, m_remotePath(Utils::toRemotePath(localEvent.localPath()))
{
}

void LocalFileOrFolderAddedEventHandler::runEventHandling()
{
    // Set name of file being handled
    markSyncing(localEvent.localPath());

	Q_ASSERT(localEvent.type() == LocalFileEvent::Added
			|| localEvent.type() == LocalFileEvent::Modified);

	if (!FactoriesStorage::instance().fileSystem().instance().isFileExists(localEvent.localPath().toStdString()))
	{
		processEventsAndQuit();
		return;
	}

	if (QFileInfo(localEvent.localPath()).isHidden())
	{
		processEventsAndQuit();
		return;
	}

	LocalCache& localCache = LocalCache::instance();

	const RemoteFileDesc fd = localCache.file(m_remotePath);
	if (!fd.isValid())
	{
		m_parentId = localCache.file(m_remotePath, true).id;
        if (m_parentId != 0)
        {
            onGetFileObjectParentIdSucceeded(m_parentId);
            return;
        }
        else
        {
            QLOG_ERROR() << "LocalFileOrFolderAddedEventHandler::run(): m_parentId == 0 for m_remotePath "
                            << m_remotePath;
            processEventsAndQuit();
            return;
        }
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
			return;
		}
	}
}

void LocalFileOrFolderAddedEventHandler::onGetFileObjectIdSucceeded(int id)
{
	FilesRestResourceRef filesRestResource = FilesRestResource::create();

	connect(filesRestResource.data(), &FilesRestResource::succeeded,
			this, &LocalFileOrFolderAddedEventHandler::onGetFileObjectSucceeded);

	connect(filesRestResource.data(), &FilesRestResource::failed,
			this, &LocalFileOrFolderAddedEventHandler::onGetFileObjectFailed);

	filesRestResource->getFileObject(id);
}

void LocalFileOrFolderAddedEventHandler::onGetFileObjectIdFailed()
{
	m_getChildrenResource = GetChildrenResource::create();

	connect(m_getChildrenResource.data(), &GetChildrenResource::getFileObjectIdSucceeded,
			this, &LocalFileOrFolderAddedEventHandler::onGetFileObjectParentIdSucceeded);

	connect(m_getChildrenResource.data(), &GetChildrenResource::getFileObjectIdFailed,
			this, &LocalFileOrFolderAddedEventHandler::onGetFileObjectParentIdFailed);

	m_getChildrenResource->getFileObjectParentId(m_remotePath);
}

void LocalFileOrFolderAddedEventHandler::onGetFileObjectSucceeded(
	RemoteFileDesc fileDesc)
{
	m_remoteFileDesc = fileDesc;

	QFileInfo fileInfo(localEvent.localPath());
	if (fileInfo.isDir())
	{
		processEventsAndQuit();
	}
	else
	{
		if (fileDesc.modifiedAt >= fileInfo.lastModified().toTime_t())
		{
			processEventsAndQuit();
		}
		else
		{
			TrashRestResourceRef trashRes = TrashRestResource::create();

			connect(trashRes.data(), &TrashRestResource::succeeded,
				this, &LocalFileOrFolderAddedEventHandler::onTrashSucceeded);

			connect(trashRes.data(), &TrashRestResource::failed,
				this, &LocalFileOrFolderAddedEventHandler::onTrashFailed);

			trashRes->trash(fileDesc.id);
		}
	}
}

void LocalFileOrFolderAddedEventHandler::onGetFileObjectFailed(const QString&)
{
	processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler::onGetFileObjectParentIdSucceeded(int id)
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
		if (QFileInfo::exists(localEvent.localPath()))
		{
			FileUploader *uploader = new FileUploader(id, localEvent.localPath(), this);

			connect(uploader, &FileUploader::succeeded,
				this, &LocalFileOrFolderAddedEventHandler::onUploadSucceeded);

			connect(uploader, &FileUploader::failed,
				this, &LocalFileOrFolderAddedEventHandler::onUploadFailed);
		}
		else
		{
			processEventsAndQuit();
		}
	}
}

void LocalFileOrFolderAddedEventHandler::onGetFileObjectParentIdFailed()
{
	processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler
	::onCreateFolderSucceeded(Drive::RemoteFileDesc fileDesc)
{
    if (LocalCache::instance().addFile(fileDesc))
    {
        Q_EMIT newRemoteFileEventExclusion(
                RemoteFileEventExclusion(RemoteFileEvent::Created, fileDesc.id));

        for(const auto entry: FileSystemHelper::entries(localEvent.localPath().toStdString()))
        {
            Q_EMIT newLocalFileEvent(
                    LocalFileEvent(LocalFileEvent::Added,
                    QDir::cleanPath(entry.absolutePath()),
                    entry.fileName()));
        }
    }

	processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler::onCreateFolderFailed(
	const QString& error)
{
    Q_EMIT failed((EventHandlerBase*) this, error);
	processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler::
	onUploadSucceeded(Drive::RemoteFileDesc fileDesc)
{
	Q_ASSERT(fileDesc.isValid());

    if (LocalCache::instance().addFile(fileDesc))
    {
        emit newRemoteFileEventExclusion(
                RemoteFileEventExclusion(RemoteFileEvent::Uploaded, fileDesc.id));
    }

	processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler::onUploadFailed(const QString& error)
{
    Q_EMIT failed((EventHandlerBase*) this, error);
	processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler::onTrashSucceeded()
{
	FilesRestResourceRef filesRestResource = FilesRestResource::create();

	connect(filesRestResource.data(), &FilesRestResource::succeeded,
		this, &LocalFileOrFolderAddedEventHandler::onRemoveSucceeded);

	connect(filesRestResource.data(), &FilesRestResource::failed,
		this, &LocalFileOrFolderAddedEventHandler::onRemoveFailed);

	filesRestResource->remove(m_remoteFileDesc.id);

	Q_EMIT newRemoteFileEventExclusion(
			RemoteFileEventExclusion(RemoteFileEvent::Trashed, m_remoteFileDesc.id));
}

void LocalFileOrFolderAddedEventHandler::onTrashFailed(const QString&)
{
	processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler::onRemoveSucceeded()
{
	onGetFileObjectIdFailed();
}

void LocalFileOrFolderAddedEventHandler::onRemoveFailed(const QString&)
{
	processEventsAndQuit();
}

// ===========================================================================

LocalFileOrFolderDeletedEventHandler::LocalFileOrFolderDeletedEventHandler(
	LocalFileEvent localEvent, QObject *parent)
	: LocalEventHandlerBase(localEvent, parent)
	, m_remoteFileObjectId(0)
{
}

void LocalFileOrFolderDeletedEventHandler::runEventHandling()
{
    // Set name of file being handled
    markSyncing(localEvent.localPath());

    Q_ASSERT(localEvent.type() == LocalFileEvent::Deleted);

    const QString localPathString = localEvent.localPath();
    const QString remotePath = Utils::toRemotePath(localPathString);

    // Handle error in path detection
    if (remotePath.isNull())
    {
        QLOG_ERROR() << "LocalFileOrFolderDeletedEventHandler::runEventHandling(): "
                        << "remotePath is null for local path [" << localPathString << "]";
        return;
    }

	// Cached:

	{
		LocalCache& cache = LocalCache::instance();
		const RemoteFileDesc file = cache.file(remotePath);
		if (file.isValid())
		{
			cache.removeFile(file);
			onGetFileObjectIdSucceeded(file.id);
			return;
		}
	}

	// Not cached:

	{
		GetChildrenResourceRef getChildrenResource = GetChildrenResource::create();

		connect(getChildrenResource.data(), &GetChildrenResource::getFileObjectIdSucceeded,
			this, &LocalFileOrFolderDeletedEventHandler::onGetFileObjectIdSucceeded);

		connect(getChildrenResource.data(), &GetChildrenResource::getFileObjectIdFailed,
			this, &LocalFileOrFolderDeletedEventHandler::onGetFileObjectIdFailed);

		getChildrenResource->getFileObjectId(remotePath);

		m_currentResource = getChildrenResource;
	}
}

void LocalFileOrFolderDeletedEventHandler::onGetFileObjectIdSucceeded(int id)
{
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
    emit failed((EventHandlerBase*) this, error);
	processEventsAndQuit();
}

// ===========================================================================

LocalFileOrFolderRenamedEventHandler::LocalFileOrFolderRenamedEventHandler(
	LocalFileEvent localEvent, QObject *parent)
	: LocalEventHandlerBase(localEvent, parent)
{
}

void LocalFileOrFolderRenamedEventHandler::runEventHandling()
{
    // Set name of file being handled
    markSyncing(localEvent.localPath());

    Q_ASSERT(localEvent.type() == LocalFileEvent::Moved);

	const QFileInfo oldFileInfo(localEvent.oldLocalPath());
	const QFileInfo newFileInfo(localEvent.localPath());

	if (newFileInfo.isHidden())
	{
		processEventsAndQuit();
		return;
	}

	Q_ASSERT(oldFileInfo.dir().absolutePath() == newFileInfo.dir().absolutePath());

	m_newName = newFileInfo.fileName();

	const QString oldRemotePath =
		Utils::toRemotePath(localEvent.oldLocalPath());

	m_currentResource = GetChildrenResource::create();

	connect(m_currentResource.data(), &GetChildrenResource::getFileObjectIdSucceeded,
		this, &LocalFileOrFolderRenamedEventHandler::onGetFileObjectIdSucceeded);

	connect(m_currentResource.data(), &GetChildrenResource::getFileObjectIdFailed,
		this, &LocalFileOrFolderRenamedEventHandler::onGetFileObjectIdFailed);

	m_currentResource->getFileObjectId(oldRemotePath);
}

void LocalFileOrFolderRenamedEventHandler::onGetFileObjectIdSucceeded(int id)
{
	FilesRestResourceRef filesRestResource = FilesRestResource::create();

	connect(filesRestResource.data(), &FilesRestResource::succeeded,
		this, &LocalFileOrFolderRenamedEventHandler::onRenameSucceeded);

	connect(filesRestResource.data(), &FilesRestResource::failed,
		this, &LocalFileOrFolderRenamedEventHandler::onRenameFailed);

	filesRestResource->rename(id, m_newName);
}

void LocalFileOrFolderRenamedEventHandler::onGetFileObjectIdFailed()
{
	Q_EMIT newLocalFileEvent(localEvent.copyTo(LocalFileEvent::Added));
	processEventsAndQuit();
}

void LocalFileOrFolderRenamedEventHandler::onRenameSucceeded(const Drive::RemoteFileDesc& fileDesc)
{
	Q_ASSERT(fileDesc.isValid());

    if (LocalCache::instance().addFile(fileDesc))
    {
        Q_EMIT newRemoteFileEventExclusion(RemoteFileEventExclusion(
                RemoteFileEvent::Renamed, fileDesc.id));
    }

	processEventsAndQuit();
}

void LocalFileOrFolderRenamedEventHandler::onRenameFailed(const QString&)
{
	processEventsAndQuit();
}

}
