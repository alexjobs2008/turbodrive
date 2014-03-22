#include "RemoteEventHandlers.h"

#include "Events/LocalFileEvent.h"
#include "QsLog/QsLog.h"
#include "APIClient/FilesService.h"
#include "APIClient/FileDownloader.h"
#include "Util/FileUtils.h"

#include <QtCore/QDir>

namespace Drive
{

RemoteEventHandlerBase::RemoteEventHandlerBase(
    RemoteFileEvent remoteEvent, QObject *parent)
    : EventHandlerBase(parent)
    , remoteEvent(remoteEvent)
{
    QLOG_TRACE() << "Thread" << this
        << "is starting processing remote file event:";

    remoteEvent.logCompact();
}

RemoteEventHandlerBase::~RemoteEventHandlerBase()
{
    QLOG_TRACE() << "Thread" << this
        << "has finished processing remote file event:";

    remoteEvent.logCompact();
}

// ===========================================================================

RemoteFolderCreatedEventHandler::RemoteFolderCreatedEventHandler(
    RemoteFileEvent remoteEvent, QObject *parent)
    : RemoteEventHandlerBase(remoteEvent, parent)
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
    
    QDir dir(localFolder);
    if (!dir.exists())
    {        
        LocalFileEventExclusion exclusion(LocalFileEvent::Added, localFolder);

        emit newLocalFileEventExclusion(exclusion);

        if (dir.mkpath("."))
            QLOG_INFO() << "Local folder created:" << localFolder;
        else
        {
            QString errorMsg = 
                QString("Local folder creation failed: %1").arg(localFolder);

            emit failed(errorMsg);
            QLOG_ERROR() << errorMsg;
        }
    }
    else
    {
        QLOG_INFO() << "Local folder already exists:" << localFolder;
    }    
    
    processEventsAndQuit();
}

void RemoteFolderCreatedEventHandler::onGetAncestorsFailed()
{
    emit failed("Failed to get the remote folder path");
    processEventsAndQuit();
}

// ===========================================================================

RemoteFileRenamedEventHandler::RemoteFileRenamedEventHandler(
    RemoteFileEvent remoteEvent, QObject *parent)
    : RemoteEventHandlerBase(remoteEvent, parent)
{
}

void RemoteFileRenamedEventHandler::run()
{
    if (!remoteEvent.isValid())
        return;

    if (remoteEvent.type != RemoteFileEvent::Renamed)
        return;

//     if (remoteEvent.fileDesc.type == RemoteFileDesc::File)
//     {
//         QLOG_ERROR() << "Remote event 'created' contains a file, not a folder";
//         return;
//     }

    GetAncestorsRestResourceRef getAncestorsRes = 
        GetAncestorsRestResource::create();

    connect(getAncestorsRes.data(), SIGNAL(succeeded(QString)),
        this, SLOT(onGetAncestorsSucceeded(QString)));

    connect(getAncestorsRes.data(), SIGNAL(failed()),
        this, SLOT(onGetAncestorsFailed()));

    getAncestorsRes->getAncestors(remoteEvent.fileDesc.id);

    exec();
}

void RemoteFileRenamedEventHandler::onGetAncestorsSucceeded(const QString& fullPath)
{
    QString newLocalPath =
        Utils::instance().remotePathToLocalPath(fullPath);

    QStringList localDirs =
        newLocalPath.split(QDir::separator(), QString::SkipEmptyParts);

    localDirs.removeLast();

    QString oldLocalPath = localDirs
        .join(QDir::separator())
        .append(QDir::separator())
        .append(remoteEvent.originName);

    bool result = false;

    QFile file(oldLocalPath);
    if (file.exists()) {
        //LocalFileEventDispatcher::instance().addExclusion(localPath);
        result = file.rename(newLocalPath);
    }

    if (result)
    {
        QLOG_INFO() << "Local file/folder renamed:"
            << remoteEvent.fileDesc.name;
    }
    else
    {
        QString errorMsg = 
            QString("Local file/folder rename failed: %1, %2")
            .arg(oldLocalPath)
            .arg(remoteEvent.fileDesc.name);

        emit failed(errorMsg);
        QLOG_ERROR() << errorMsg;
    }

    processEventsAndQuit();
}

void RemoteFileRenamedEventHandler::onGetAncestorsFailed()
{
    emit failed("Failed to get the remote folder path");
    processEventsAndQuit();
}

// ===========================================================================

RemoteFileTrashedEventHandler::RemoteFileTrashedEventHandler(
    RemoteFileEvent remoteEvent, QObject *parent)
    : RemoteEventHandlerBase(remoteEvent, parent)
{
}
     
void RemoteFileTrashedEventHandler::run()
{
    if (!remoteEvent.isValid())
        return;

    if (remoteEvent.type != RemoteFileEvent::Trashed)
        return;

    onGetAncestorsSucceeded(remoteEvent.fileDesc.originalPath);
    
//  Latest backend changes (as of 30.12.2013) made the following obsolete:
//
//     GetAncestorsRestResourceRef getAncestorsRes = 
//         GetAncestorsRestResource::create();
// 
//     connect(getAncestorsRes.data(), SIGNAL(succeeded(QString)),
//         this, SLOT(onGetAncestorsSucceeded(QString)));
// 
//     connect(getAncestorsRes.data(), SIGNAL(failed()),
//         this, SLOT(onGetAncestorsFailed(QString)));
// 
//     getAncestorsRes->getAncestors(remoteEvent.fileDesc.parentId);
// 
//     exec();
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
        processEventsAndQuit();
    }

    bool isRemoved = false;

    if (fileInfo.isFile() || fileInfo.isSymLink())
    {
        LocalFileEventExclusion exclusion(LocalFileEvent::Deleted, localPath);
        emit newLocalFileEventExclusion(exclusion);

        isRemoved = QFile::remove(localPath);
    }
    else // dir or bundle
    {
        LocalFileEventExclusion exclusion(LocalFileEvent::Deleted
            , localPath
            , LocalFileEventExclusion::PartialMatch);

        emit newLocalFileEventExclusion(exclusion);

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

    processEventsAndQuit();
}

void RemoteFileTrashedEventHandler::onGetAncestorsFailed()
{
    emit failed("Failed to get the remote file object path");    
    processEventsAndQuit();
}

// ===========================================================================

RemoteFileUploadedEventHandler::RemoteFileUploadedEventHandler(
    RemoteFileEvent remoteEvent, QObject *parent)
    : RemoteEventHandlerBase(remoteEvent, parent)
    , localFilePath(QString())
{
}

void RemoteFileUploadedEventHandler::run()
{
    QLOG_INFO() << "RemoteFileUploadedEventHandler::run(): "
        << this;

    remoteEvent.logCompact();

    if (!remoteEvent.isValid())
    {
        QLOG_ERROR() << "Remote file event is not valid:";
        remoteEvent.logCompact();
        return;
    }

    if (remoteEvent.type != RemoteFileEvent::Uploaded)
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
        this, SLOT(onGetAncestorsFailed()));

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
            QLOG_TRACE() << "Local file is newer than remote file";
            
            // local file is newer than the remote file,
            // so no need to download. The local file should be uploaded
            // and substitute the remote file

            LocalFileEvent event;
            event.type = LocalFileEvent::Modified;
            event.timestamp = QDateTime::currentDateTime().toTime_t();
            event.dir = QDir::fromNativeSeparators(
                fileInfo.absoluteDir().absolutePath());
            event.filePath = fileInfo.fileName();

            emit newLocalFileEvent(event);            
            processEventsAndQuit();
            emit succeeded();
            return;
        }
        else if (fileInfo.lastModified().toTime_t() 
            == remoteEvent.fileDesc.modifiedAt)
        {
            QLOG_TRACE() << "Local file timestamp == remote file timestamp";
            emit succeeded();
            processEventsAndQuit();
            return;
        }
    }

    // Local file either doesn't exist or is older

    downloader = new FileDownloader(remoteEvent.fileDesc.id,
        localFilePath, remoteEvent.fileDesc.modifiedAt, this);

    connect(downloader, SIGNAL(succeeded()),
        this, SLOT(onDownloadSucceeded()));

    connect(downloader, SIGNAL(failed(QString)),
        this, SLOT(onDownloadFailed(QString)));

    LocalFileEventExclusion
        addedEventExclusion(LocalFileEvent::Added, localFilePath);

    LocalFileEventExclusion
        modifiedEventExclusion(LocalFileEvent::Modified, localFilePath);

    emit newLocalFileEventExclusion(addedEventExclusion);
    emit newLocalFileEventExclusion(modifiedEventExclusion);

    downloader->limitSpeed(50);
    downloader->download();    

}

void RemoteFileUploadedEventHandler::onGetAncestorsFailed()
{
    emit failed("Failed to get the remote file path");    
    processEventsAndQuit();
}

void RemoteFileUploadedEventHandler::onDownloadSucceeded()
{    
    QLOG_TRACE() << "Download succeeded";
    processEventsAndQuit();
}

void RemoteFileUploadedEventHandler::onDownloadFailed(const QString& error)
{    
    QLOG_ERROR() << "Download failed";
    emit failed(
        QString(tr("File uploaded event handler failed: %1")).arg(error));
    
    processEventsAndQuit();
}

// ===========================================================================

RemoteFileOrFolderRestoredEventHandler::RemoteFileOrFolderRestoredEventHandler(
    RemoteFileEvent remoteEvent,
    QObject *parent)
    : RemoteEventHandlerBase(remoteEvent, parent)
{
}

void RemoteFileOrFolderRestoredEventHandler::run()
{
    QLOG_INFO() << "RemoteFileRestoredEventHandler::run(): "
        << this;

    remoteEvent.logCompact();

    if (!remoteEvent.isValid())
    {
        QLOG_ERROR() << "Remote file event is not valid:";
        remoteEvent.logCompact();
        return;
    }

    if (remoteEvent.type != RemoteFileEvent::Restored)
        return;

    if (remoteEvent.fileDesc.type == RemoteFileDesc::Folder)
    {
        // 1. fire folder created remote event
        // 2. if has children, then getChildren and create remote events for them
        
        RemoteFileEvent newRemoteEvent(remoteEvent);
        newRemoteEvent.type = RemoteFileEvent::Created;
        emit newPriorityRemoteFileEvent(newRemoteEvent);

        if (remoteEvent.fileDesc.hasChildren)
        {
            GetChildrenResourceRef getChildrenRes = 
                GetChildrenResource::create();

            connect(getChildrenRes.data(), SIGNAL(succeeded(QList<Drive::RemoteFileDesc>)),
                this, SLOT(onGetChildrenSucceeded(QList<Drive::RemoteFileDesc>)));

            connect(getChildrenRes.data(), SIGNAL(failed()),
                this, SLOT(onGetChildrenFailed()));

            getChildrenRes->getChildren(remoteEvent.fileDesc.id);

            exec();
        }        
    }
    else if (remoteEvent.fileDesc.type == RemoteFileDesc::File)
    {
        RemoteFileEvent newRemoteEvent(remoteEvent);
        newRemoteEvent.type = RemoteFileEvent::Uploaded;
        emit newPriorityRemoteFileEvent(newRemoteEvent);

        emit succeeded();
        processEventsAndQuit();
    }
}

void RemoteFileOrFolderRestoredEventHandler::onGetChildrenSucceeded(
    QList<RemoteFileDesc> list)
{
    foreach (RemoteFileDesc fileDesc, list)
    {
        RemoteFileEvent newRemoteEvent;
        newRemoteEvent.fileDesc = fileDesc;
        newRemoteEvent.timestamp = remoteEvent.timestamp;
        newRemoteEvent.unixtime = remoteEvent.unixtime;
        newRemoteEvent.projectId = remoteEvent.projectId;
        newRemoteEvent.workspaceId = remoteEvent.workspaceId;

        if (fileDesc.type == RemoteFileDesc::Folder)
        {
            newRemoteEvent.type = RemoteFileEvent::Restored;
        }
        else // i.e. fileDesc.type == RemoteFileDesc::File
        {
            newRemoteEvent.type = RemoteFileEvent::Uploaded;
        }

        emit newPriorityRemoteFileEvent(newRemoteEvent);
    }

    emit succeeded();
    processEventsAndQuit();
}

void RemoteFileOrFolderRestoredEventHandler::onGetChildrenFailed()
{
    QLOG_ERROR() << this << "Failed to get children.";
    emit failed("Failed to get children.");
    processEventsAndQuit();
}

// ===========================================================================

RemoteFileCopiedEventHandler::RemoteFileCopiedEventHandler(
    RemoteFileEvent remoteEvent, QObject *parent)
    : RemoteEventHandlerBase(remoteEvent, parent)
{
}

void RemoteFileCopiedEventHandler::run()
{
    QLOG_INFO() << "RemoteFileCopiedEventHandler::run(): "
        << this;

    remoteEvent.logCompact();

    if (!remoteEvent.isValid())
    {
        QLOG_ERROR() << "Remote file event is not valid:";
        remoteEvent.logCompact();
        return;
    }

    if (remoteEvent.type != RemoteFileEvent::Copied)
        return;

    // 1. get source remote path
    // 2. get target file object
    // 3. get target remote path
    // 4. translate remote paths to local paths
    // 5. do the copy op

    GetAncestorsRestResourceRef getSourceAncestorsRes = 
        GetAncestorsRestResource::create();

    connect(getSourceAncestorsRes.data()
        , SIGNAL(succeeded(QString))
        , this
        , SLOT(onGetAncestorsSucceeded(QString)));

    connect(getSourceAncestorsRes.data()
        , SIGNAL(failed())
        , this
        , SLOT(onGetAncestorsFailed(QString)));

    getSourceAncestorsRes->getAncestors(remoteEvent.sourceId);

    FilesRestResourceRef filesRestResource = FilesRestResource::create();

    connect(filesRestResource.data()
        , SIGNAL(getFileObjectSucceeded(Drive::RemoteFileDesc))
        , this
        , SLOT(onGetFileObjectSucceeded(Drive::RemoteFileDesc)));

    connect(filesRestResource.data(), SIGNAL(failed(QString)),
        this, SLOT(onGetFileObjectFailed(QString)));

    filesRestResource->getFileObject(remoteEvent.targetId);

    exec();
}

void RemoteFileCopiedEventHandler::onGetAncestorsSucceeded(QString)
{

}

void RemoteFileCopiedEventHandler::onGetAncestorsFailed(QString)
{

}

}
