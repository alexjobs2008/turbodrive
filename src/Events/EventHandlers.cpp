#include "EventHandlers.h"

#include "APIClient/FileDownloader.h"
#include "APIClient/FileUploader.h"
#include "QsLog/QsLog.h"
#include "Util/FileUtils.h"
#include "Events/LocalDispatcher.h"

#include <QtCore/QDir>

namespace Drive
{

RemoteFolderCreatedEventHandler::RemoteFolderCreatedEventHandler(
    RemoteFileEvent remoteEvent, QObject *parent)
    : QThread(parent)
    , remoteEvent(remoteEvent)
{
}

void RemoteFolderCreatedEventHandler::run()
{   
    if (!remoteEvent.isValid())
    {
        QLOG_ERROR() << "remote event is not valid";
        return;
    }

    if (remoteEvent.type != RemoteFileEvent::Created)
    {
        QLOG_ERROR() << "remote event type:" << remoteEvent.type
            << ", should be" << RemoteFileEvent::Created;
        return;
    }
    
    if (remoteEvent.fileDesc.type == RemoteFileDesc::File)
    {
        QLOG_ERROR() << "Remote event 'created' contains a file, not a folder";
        return;
    }

    GetAncestorsRestResourceRef getAncestorsRes = 
        GetAncestorsRestResource::create();

    connect(getAncestorsRes.data(), SIGNAL(succeeded(QString)),
        this, SLOT(onGetAncestorsSucceeded(QString)));

    getAncestorsRes->getAncestors(remoteEvent.fileDesc.id);
       
    exec();
}

void RemoteFolderCreatedEventHandler::onGetAncestorsSucceeded(const QString& fullPath)
{
    QString localFolder =
        Utils::instance().remotePathToLocalPath(fullPath);

    bool result = false;
    
    QDir dir(localFolder);
    if (!dir.exists()) {
        LocalFileEventDispatcher::instance().addExclusion(localFolder);
        result = dir.mkpath(".");
    }

    if (result)    
        QLOG_INFO() << "Local folder created:" << localFolder;
    else
    {
        QString errorMsg = 
            QString("Local folder creation failed: %1").arg(localFolder);

        emit failed(errorMsg);
        QLOG_ERROR() << errorMsg;
    }
    
    quit();
    wait();
}

void RemoteFolderCreatedEventHandler::onGetAncestorsFailed()
{
    emit failed("Failed to get the remote folder path");
    quit();
    wait();
}

// ---------------------------------------------------------------------------

RemoteFileRenamedEventHandler::RemoteFileRenamedEventHandler(
    RemoteFileEvent remoteEvent,
    QObject *parent)
    : QThread(parent)
    , remoteEvent(remoteEvent)
{
}

void RemoteFileRenamedEventHandler::run()
{
    if (!remoteEvent.isValid())
        return;

    if (remoteEvent.type != RemoteFileEvent::Created)
        return;

    if (remoteEvent.fileDesc.type == RemoteFileDesc::File)
    {
        QLOG_ERROR() << "Remote event 'created' contains a file, not a folder";
        return;
    }

    GetAncestorsRestResourceRef getAncestorsRes = 
        GetAncestorsRestResource::create();

    connect(getAncestorsRes.data(), SIGNAL(succeeded(QString)),
        this, SLOT(onGetAncestorsSucceeded(QString)));

    getAncestorsRes->getAncestors(remoteEvent.fileDesc.id);

    exec();
}

void RemoteFileRenamedEventHandler::onGetAncestorsSucceeded(const QString& fullPath)
{
//     QString localPath =
//         Utils::instance().remotePathToLocalPath(fullPath);
// 
//     bool result = false;
// 
//     QFile file(localPath);
//     if (file.exists()) {
//         LocalFileEventDispatcher::instance().addExclusion(localPath);
//         result = file.rename(remoteEvent.fileDesc.name);
//     }
// 
//     if (result)
//     {
//         QLOG_INFO() << "Local file/folder renamed:"
//             << remoteEvent.fileDesc.name;
//     }
//     else
//     {
//         QString errorMsg = 
//             QString("Local file/folder rename failed: %1").arg(localPath);
// 
//         emit failed(errorMsg);
//         QLOG_ERROR() << errorMsg;
//     }

    quit();
    wait();
}

void RemoteFileRenamedEventHandler::onGetAncestorsFailed()
{
    emit failed("Failed to get the remote folder path");
    quit();
    wait();
}

// ---------------------------------------------------------------------------

RemoteFileTrashedEventHandler::RemoteFileTrashedEventHandler(
    RemoteFileEvent remoteEvent, QObject *parent)
    : QThread(parent)
    , remoteEvent(remoteEvent)
{
}
     
void RemoteFileTrashedEventHandler::run()
{
    if (!remoteEvent.isValid())
        return;

    if (remoteEvent.type != RemoteFileEvent::Trashed)
        return;

//     if (remoteEvent.fileDesc.type == RemoteFileDesc::File)
//     {
//         QFileInfo fileInfo()
// 
//         return;
//     }

    GetAncestorsRestResourceRef getAncestorsRes = 
        GetAncestorsRestResource::create();

    connect(getAncestorsRes.data(), SIGNAL(succeeded(QString)),
        this, SLOT(onGetAncestorsSucceeded(QString)));

    connect(getAncestorsRes.data(), SIGNAL(failed()),
        this, SLOT(onGetAncestorsFailed(QString)));

    getAncestorsRes->getAncestors(remoteEvent.fileDesc.parentId);

    exec();
}

void RemoteFileTrashedEventHandler::onGetAncestorsSucceeded(
    const QString& fullPath)
{
    QLOG_TRACE() << "file object remote path: " << fullPath;

    localPath = Utils::instance().remotePathToLocalPath(fullPath, true);
    localPath.append(remoteEvent.fileDesc.name);
    
    QLOG_TRACE() << "file/folder local path: " << localPath;

    QFileInfo fileInfo(localPath);
    if (!fileInfo.exists())
    {
        QLOG_TRACE() << "File/folder doesn't exist: nothing to delete.";
        quit();
        wait();
    }

    bool isRemoved = false;

    if (fileInfo.isFile() || fileInfo.isSymLink())
    {
        LocalFileEventDispatcher::instance().addExclusion(localPath);
        isRemoved = QFile::remove(localPath);
    }
    else // dir or bundle
    {
        isRemoved = FileSystemHelper::removeDirWithSubdirs(localPath);
    }

    if (isRemoved)
    {
        QLOG_INFO() << "Successfully removed: " << localPath;
    }
    else
    {
        QLOG_INFO() << "Failed to remove: " << localPath;
    }

    quit();
    wait();
}

void RemoteFileTrashedEventHandler::onGetAncestorsFailed()
{
    emit failed("Failed to get the remote file object path");    
    quit();
    wait();
}

// ---------------------------------------------------------------------------

RemoteFileUploadedEventHandler::RemoteFileUploadedEventHandler(
    RemoteFileEvent remoteEvent,
    QObject *parent)
    : QThread(parent)
    , remoteEvent(remoteEvent)
    , localFilePath(QString())
{
}

void RemoteFileUploadedEventHandler::run()
{
    if (!remoteEvent.isValid())
        return;

    if (remoteEvent.type != RemoteFileEvent::Uploaded
        && remoteEvent.type != RemoteFileEvent::Restored)
        return;

    if (remoteEvent.fileDesc.type == RemoteFileDesc::Folder)
    {
        QLOG_ERROR() <<
            "Remote event 'uploaded' contains a folder, not a file";
        return;
    }

    GetAncestorsRestResourceRef getAncestorsRes = 
        GetAncestorsRestResource::create();

    connect(getAncestorsRes.data(), SIGNAL(succeeded(QString)),
        this, SLOT(onGetAncestorsSucceeded(QString)));

    connect(getAncestorsRes.data(), SIGNAL(failed()),
        this, SLOT(onGetAncestorsFailed(QString)));

    getAncestorsRes->getAncestors(remoteEvent.fileDesc.id);

    exec();
}

void RemoteFileUploadedEventHandler::onGetAncestorsSucceeded(
    const QString& fullPath)
{
    QLOG_TRACE() << "file remote path: " << fullPath;

    localFilePath = Utils::instance().remotePathToLocalPath(fullPath);

    QLOG_TRACE() << "file local path: " << localFilePath;

    // - check if file exist and if it has newer timestamp
    // - another possible check: if it's a local dir with the same name,
    //   but that should never happen

    QFileInfo fileInfo(localFilePath);
    if (fileInfo.exists())
    {
        if (fileInfo.lastModified().toTime_t()
            > remoteEvent.fileDesc.modifiedAt)
        {
            // local file is newer than the remote file,
            // so no need to download. The local file should be uploaded
            // and substitute the remote file

            LocalFileEvent event;
            event.type = LocalFileEvent::Modified;
            event.timestamp = QDateTime::currentDateTime().toTime_t();
            event.dir = QDir::fromNativeSeparators(
                fileInfo.absoluteDir().absolutePath());
            event.fileName() = fileInfo.fileName();

            emit newLocalEvent(event);
            return;
        }
    }    

    downloader = new FileDownloader(remoteEvent.fileDesc.id,
        localFilePath, remoteEvent.fileDesc.modifiedAt, this);
    
    connect(downloader, SIGNAL(succeeded()),
        this, SLOT(onDownloadSucceeded()));

    connect(downloader, SIGNAL(failed(QString)),
        this, SLOT(onDownloadFailed(QString)));
    
    LocalFileEventDispatcher::instance().addExclusion(localFilePath);
    
    downloader->limitSpeed(50);
    downloader->download();
}

void RemoteFileUploadedEventHandler::onGetAncestorsFailed()
{
    emit failed("Failed to get the remote file path");    
    quit();
    wait();    
}

void RemoteFileUploadedEventHandler::onDownloadSucceeded()
{    
    QLOG_TRACE() << "Download succeeded";
    emit removeLocalFileEventExclusion(localFilePath);
    quit();
    wait();
}

void RemoteFileUploadedEventHandler::onDownloadFailed(const QString& error)
{    
    QLOG_ERROR() << "Download failed";
    emit removeLocalFileEventExclusion(localFilePath);
    emit failed(
        QString(tr("File uploaded event handler failed: %1")).arg(error));
    quit();
    wait();
}


// ---------------------------------------------------------------------

LocalFileOrFolderAddedEventHandler::LocalFileOrFolderAddedEventHandler(
    LocalFileEvent localEvent, QObject *parent)
    : QThread(parent)
    , localEvent(localEvent)
{
}

void LocalFileOrFolderAddedEventHandler::run()
{
    if (localEvent.type != LocalFileEvent::Added)
        return;

    QString remotePath = Utils::localPathToRemotePath(localEvent.localPath());

    GetChildrenResourceRef getChildrenResource = GetChildrenResource::create();

    connect(getChildrenResource.data(), SIGNAL(getFileObjectIdSucceeded(int)),
        this, SLOT(onGetFileObjectIdSucceeded(int)));

    connect(getChildrenResource.data(), SIGNAL(getFileObjectIdFailed()),
        this, SLOT(onGetFileObjectIdFailed()));

    getChildrenResource->getFileObjectParentId(remotePath);    

    exec();
}

void LocalFileOrFolderAddedEventHandler::onGetFileObjectIdSucceeded(int id)
{
    QLOG_TRACE() <<
        "LocalFileOrFolderAddedEventHandler file object id:" << id;

    QFileInfo fileInfo(localEvent.localPath());    
    if (fileInfo.isDir())
    {
        CreateRestResourceRef createRestResource =
            CreateRestResource::create();

        connect(createRestResource.data(), SIGNAL(succeeded()),
            this, SLOT(onCreateFolderSucceeded()));

        connect(createRestResource.data(), SIGNAL(failed(QString)),
            this, SLOT(onCreateFolderFailed(QString)));

        createRestResource->createFolder(id, localEvent.fileName());
    }
    else
    {
        FileUploader *uploader = new FileUploader(this);
        
        connect(uploader, SIGNAL(succeeded()),
            this, SLOT(onUploadSucceeded()));

        connect(uploader, SIGNAL(failed(QString)),
            this, SLOT(onUploadFailed(QString)));

        uploader->uploadFile(id, localEvent.localPath());

    }
}

void LocalFileOrFolderAddedEventHandler::onGetFileObjectIdFailed()
{
    QLOG_ERROR() << "Failed to create remote file object: no parent id";
    quit();
    wait();
}

void LocalFileOrFolderAddedEventHandler::onCreateFolderSucceeded()
{
    QLOG_TRACE() << "Remote folder created";
    quit();
    wait();
}

void LocalFileOrFolderAddedEventHandler::onCreateFolderFailed(
    const QString& error)
{
    QLOG_TRACE() << "Failed to create a remote folder";
    emit failed(error);
    quit();
    wait();
}

void LocalFileOrFolderAddedEventHandler::onUploadSucceeded()
{
    QLOG_TRACE() << "File uploaded";
    quit();
    wait();
}

void LocalFileOrFolderAddedEventHandler::onUploadFailed(const QString& error)
{
    QLOG_TRACE() << "Failed to upload file." << error;
    emit failed(error);
    quit();
    wait();
}

// ---------------------------------------------------------------------

LocalFileOrFolderDeletedEventHandler::LocalFileOrFolderDeletedEventHandler(
    LocalFileEvent localEvent, QObject *parent)
    : QThread(parent)
    , localEvent(localEvent)
{
}

void LocalFileOrFolderDeletedEventHandler::run()
{
    if (localEvent.type != LocalFileEvent::Deleted)
        return;

    QString remotePath = Utils::localPathToRemotePath(localEvent.localPath());

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
}

void LocalFileOrFolderDeletedEventHandler::onGetFileObjectIdFailed()
{
    QLOG_ERROR() << "Failed to trash remote file object: no id";
    quit();
    wait();
}

void LocalFileOrFolderDeletedEventHandler::onTrashSucceeded()
{
    QLOG_TRACE() << "Remote filed object trashed.";
    quit();
    wait();
}

void LocalFileOrFolderDeletedEventHandler::onTrashFailed(const QString& error)
{
    QLOG_TRACE() << "Failed to trash a remote file object";
    emit failed(error);
    quit();
    wait();
}

// ---------------------------------------------------------------------

LocalFileOrFolderRenamedEventHandler::LocalFileOrFolderRenamedEventHandler(
    LocalFileEvent localEvent, QObject *parent)
    : QThread(parent)
    , localEvent(localEvent)
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
//     if (!parentId)
//     {
//         parentId = id;
//         QLOG_TRACE() <<
//             "LocalFileOrFolderAddedEventHandler parent file object id:" << id;
// 
//         QString oldRemotePath =
//             Utils::localPathToRemotePath(localEvent.oldLocalPath());
// 
//         // reusing the current rest resource
//         getChildrenResource->getFileObjectId(oldRemotePath);
// 
//         return;
//     }

    this->id = id;
     
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
    QLOG_ERROR() << "Failed to rename remote file object: no id";
    quit();
    wait();
}

void LocalFileOrFolderRenamedEventHandler::onRenameSucceeded()
{
    QLOG_TRACE() << "Remote file object rename succeeded: " << id << newName;

//  TODO: add remote file event exclusion
//     RemoteFileDesc fileDesc;
//     fileDesc.id = id;
//     fileDesc.name = newName;
// 
//     RemoteFileEvent remoteEvent;
//     remoteEvent.type = RemoteFileEvent::Renamed;
//     
//     emit ...

    quit();
    wait();
}

void LocalFileOrFolderRenamedEventHandler::onRenameFailed(const QString& error)
{
    QLOG_ERROR() << "Failed to rename remote file object: "
        << id << newName << error;
    quit();
    wait();
}

}