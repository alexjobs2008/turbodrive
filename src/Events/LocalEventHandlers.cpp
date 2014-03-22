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

bool LocalEventHandlerBase::localFileExists()
{
    QFileInfo fileInfo(localEvent.localPath());

    if (!fileInfo.exists())
    {
        QLOG_ERROR() << this << ": local file/folder doesn't exist: "
            << localEvent.localPath();
        
        return false;
    }
    else
    {
        QString fileObjectTypeName = "N/A";
        if (fileInfo.isDir())
        {
            fileObjectTypeName = "folder";
        }
        else if (fileInfo.isSymLink())
        {
            fileObjectTypeName = "symlink";
        }
        else if (fileInfo.isBundle())
        {
            fileObjectTypeName = "bundle";
        }
        else if (fileInfo.isFile())
        {
            fileObjectTypeName = "file";
        }        
        
        QLOG_TRACE() << this << ": Local" << fileObjectTypeName << "exists: "
            << localEvent.localPath();

        return true;
    }
}   

// ===========================================================================

LocalFileOrFolderAddedEventHandler::LocalFileOrFolderAddedEventHandler(
    LocalFileEvent localEvent, QObject *parent)
    : LocalEventHandlerBase(localEvent, parent)
    , objParentId(0)
{
    QLOG_TRACE() << "LocalFileOrFolderAddedEventHandler created" << this;
}

void LocalFileOrFolderAddedEventHandler::run()
{
    QLOG_TRACE() << this << "run";

    if (localEvent.type != LocalFileEvent::Added
        && localEvent.type != LocalFileEvent::Modified)
        return;

    if (!localFileExists())
    {
        QLOG_ERROR() << "Local file or folder doesn't exist";
        processEventsAndQuit();
    }
    else
    {
        remotePath = Utils::localPathToRemotePath(localEvent.localPath());   
        
        // TODO: don't process hidden files

        // Cached:

        LocalCache& localCache = LocalCache::instance();

        RemoteFileDesc fd = localCache.fileDesc(remotePath);
        if (!fd.isValid())
        {
            QLOG_TRACE() << this
                << "No RemoteFileObject found in cache for "
                << remotePath;

            objParentId = localCache.id(remotePath, true);

            if (objParentId)
            {
                onGetFileObjectParentIdSucceeded(objParentId);
                exec();
                return;
            }
            else
            {
                QLOG_ERROR() << this << "No parent id cached";
                emit failed("No parent id cached");
                processEventsAndQuit();
                return;
            }
        }
        else
        {
            QLOG_TRACE() << this
                << "RemoteFileObject found is found in cache"
                << remotePath;

            if (fd.type == RemoteFileDesc::Folder)
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

        // Non-cached:

        // 1. check if file object with the same path and name exists
        // 2. if exists, then check timestamp
        // 3. if timestamp is older, then: delete old, upload new
        //    if newer, then do nothing
        // 4. if doesn't exist, then upload

        

        GetChildrenResourceRef getChildrenResource =
            GetChildrenResource::create();

        connect(getChildrenResource.data(), SIGNAL(getFileObjectIdSucceeded(int)),
            this, SLOT(onGetFileObjectIdSucceeded(int)));

        connect(getChildrenResource.data(), SIGNAL(getFileObjectIdFailed()),
            this, SLOT(onGetFileObjectIdFailed()));

        getChildrenResource->getFileObjectId(remotePath);

        exec();
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

//     GetChildrenResourceRef getChildrenResource =
//         GetChildrenResource::create();

    getChildrenResource =
        GetChildrenResource::create();

    connect(getChildrenResource.data()
        , SIGNAL(getFileObjectIdSucceeded(int))
        , this
        , SLOT(onGetFileObjectParentIdSucceeded(int)));
    
    connect(getChildrenResource.data()
        , SIGNAL(getFileObjectIdFailed())
        , this
        , SLOT(onGetFileObjectParentIdFailed()));

    getChildrenResource->getFileObjectParentId(remotePath);
}

void LocalFileOrFolderAddedEventHandler::onGetFileObjectSucceeded(
    RemoteFileDesc fileDesc)
{
    // file object exists
    // if it's a folder, then do nothing and quit
    // if it's a file, then check timestamp
    //    if remote's one is newer, then do nothing and quit
    //    else delete and upload

    remotefileDesc = fileDesc;

    QLOG_TRACE() << this << "getFileObjectSucceeded:" << fileDesc.id << fileDesc.name << fileDesc.modifiedAt;

    QFileInfo fileInfo(localEvent.localPath());    
    if (fileInfo.isDir())
    {
        QLOG_INFO() << "Remote folder already exists: "
            << remotePath;

        processEventsAndQuit();
    }
    else
    {
        if (fileDesc.modifiedAt >= fileInfo.lastModified().toTime_t())
        {
            QLOG_INFO() << this
                << "Remote file object is not older than local, no need to update it."
                << remotePath;
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
    QLOG_TRACE() << this
        << "getFileObjectParentIdSucceeded, file object Parent id:" << id;

    QFileInfo fileInfo(localEvent.localPath());    
    if (fileInfo.isDir())
    {
        CreateRestResourceRef createRestResource =
            CreateRestResource::create();

        connect(createRestResource.data()
            , SIGNAL(succeeded(Drive::RemoteFileDesc))
            , this
            , SLOT(onCreateFolderSucceeded(Drive::RemoteFileDesc)));

        connect(createRestResource.data(), SIGNAL(failed(QString)),
            this, SLOT(onCreateFolderFailed(QString)));

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

    LocalCache::instance().onNewFileDesc(fileDesc);
    
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
    QLOG_TRACE() << this << "File uploaded:" << remotePath;
    
    if (fileDesc.isValid())
    {
        LocalCache::instance().onNewFileDesc(fileDesc);

        RemoteFileEventExclusion exclusion(RemoteFileEvent::Uploaded, fileDesc.id);
        emit newRemoteFileEventExclusion(exclusion);
    }    

    processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler::onUploadFailed(const QString& error)
{
    QLOG_ERROR() << this << "Failed to upload file:" << remotePath << error;
    emit failed(error);
    processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler::onTrashSucceeded()
{
    QLOG_TRACE() << this << "trash succeeded for id" << remotefileDesc.id;

    FilesRestResourceRef filesRestResource = FilesRestResource::create();

    connect(filesRestResource.data(), SIGNAL(succeeded()),
        this, SLOT(onRemoveSucceeded()));

    connect(filesRestResource.data(), SIGNAL(failed(QString)),
        this, SLOT(onRemoveFailed(QString)));

    filesRestResource->remove(remotefileDesc.id);

    RemoteFileEventExclusion
        exclusion(RemoteFileEvent::Trashed, remotefileDesc.id);
    emit newRemoteFileEventExclusion(exclusion);
}

void LocalFileOrFolderAddedEventHandler::onTrashFailed(const QString& error)
{
    QLOG_ERROR() << this << ": failed to trash remote file object:" << remotefileDesc.id;
    processEventsAndQuit();
}

void LocalFileOrFolderAddedEventHandler::onRemoveSucceeded()
{    
    QLOG_TRACE() << this << "remove succeeded:" << remotefileDesc.id;
    onGetFileObjectIdFailed();
}

void LocalFileOrFolderAddedEventHandler::onRemoveFailed(const QString& error)
{
    QLOG_ERROR() << this << ": failed to remove remote file object:" << remotefileDesc.id;
    processEventsAndQuit();
}

// ===========================================================================

LocalFileOrFolderDeletedEventHandler::LocalFileOrFolderDeletedEventHandler(
    LocalFileEvent localEvent, QObject *parent)
    : LocalEventHandlerBase(localEvent, parent)
    , remoteFileObjectId(0)
{
}

void LocalFileOrFolderDeletedEventHandler::run()
{
    if (localEvent.type != LocalFileEvent::Deleted)
        return;

    QString remotePath = Utils::localPathToRemotePath(localEvent.localPath());

    // Cached:

    int objId = LocalCache::instance().id(remotePath);

    if (objId)
    {
        onGetFileObjectIdSucceeded(objId);
        exec();
        return;
    }

    // Not cached:

    GetChildrenResourceRef getChildrenResource = GetChildrenResource::create();

    connect(getChildrenResource.data(), SIGNAL(getFileObjectIdSucceeded(int)),
        this, SLOT(onGetFileObjectIdSucceeded(int)));

    connect(getChildrenResource.data(), SIGNAL(getFileObjectIdFailed()),
        this, SLOT(onGetFileObjectIdFailed()));

    getChildrenResource->getFileObjectId(remotePath);

    exec();
}

void LocalFileOrFolderDeletedEventHandler::onGetFileObjectIdSucceeded(int id)
{
    QLOG_TRACE() <<
        "LocalFileOrFolderDeletedEventHandler file object id:" << id;

    QFileInfo fileInfo(localEvent.localPath());    

    TrashRestResourceRef trashRestResource = TrashRestResource::create();

    connect(trashRestResource.data(), SIGNAL(succeeded()),
        this, SLOT(onTrashSucceeded()));

    connect(trashRestResource.data(), SIGNAL(failed(QString)),
        this, SLOT(onTrashFailed(QString)));

    trashRestResource->trash(id);

    remoteFileObjectId = id;
}

void LocalFileOrFolderDeletedEventHandler::onGetFileObjectIdFailed()
{
    QLOG_ERROR() << "Failed to trash remote file object: no id";
    processEventsAndQuit();
}

void LocalFileOrFolderDeletedEventHandler::onTrashSucceeded()
{
    QLOG_TRACE() << "Remote filed object trashed.";    
    
    if (remoteFileObjectId)
    {
        RemoteFileEventExclusion exclusion(RemoteFileEvent::Trashed
            , remoteFileObjectId);
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
        Utils::localPathToRemotePath(localEvent.oldLocalPath());

        //     QString localFolderAbsolutePath = newFileInfo.dir().absolutePath();
        //     QString folderRemotePath =
        //         Utils::localPathToRemotePath(localFolderAbsolutePath);

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