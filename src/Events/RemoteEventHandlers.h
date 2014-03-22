#ifndef REMOTE_EVENT_HANDLERS
#define REMOTE_EVENT_HANDLERS

#include "EventHandlerBase.h"
#include "APIClient/ApiTypes.h"

namespace Drive
{

class RemoteEventHandlerBase : public EventHandlerBase
{
    Q_OBJECT
public:
    RemoteEventHandlerBase(RemoteFileEvent remoteEvent,
        QObject *parent = 0);

    virtual ~RemoteEventHandlerBase();

protected:
    RemoteFileEvent remoteEvent;
};

class RemoteFolderCreatedEventHandler : public RemoteEventHandlerBase
{
    Q_OBJECT
public:
    RemoteFolderCreatedEventHandler(RemoteFileEvent remoteEvent,
        QObject *parent = 0);

protected:
    void run();

private slots:
    void onGetAncestorsSucceeded(const QString& fullPath);
    void onGetAncestorsFailed();
};

class RemoteFileRenamedEventHandler : public RemoteEventHandlerBase
{
    Q_OBJECT
public:
    RemoteFileRenamedEventHandler(RemoteFileEvent remoteEvent,
        QObject *parent = 0);

protected:
    void run();

private slots:
    void onGetAncestorsSucceeded(const QString& fullPath);
    void onGetAncestorsFailed();
};

class RemoteFileTrashedEventHandler : public RemoteEventHandlerBase
{
    Q_OBJECT
public:
    RemoteFileTrashedEventHandler(RemoteFileEvent remoteEvent,
        QObject *parent = 0);

protected:
    void run();

private slots:
    void onGetAncestorsSucceeded(const QString& fullPath);
    void onGetAncestorsFailed();

private:
    QString localPath;
};


class FileDownloader;

class RemoteFileUploadedEventHandler : public RemoteEventHandlerBase
{
    Q_OBJECT
public:
    RemoteFileUploadedEventHandler(RemoteFileEvent remoteEvent,
        QObject *parent = 0);

protected:
    void run();

private slots:
    void onGetAncestorsSucceeded(const QString& fullPath);
    void onGetAncestorsFailed();
    void onDownloadSucceeded();
    void onDownloadFailed(const QString& error);

private:
    FileDownloader *downloader;
    QString localFilePath;
};

class RemoteFileOrFolderRestoredEventHandler : public RemoteEventHandlerBase
{
    Q_OBJECT
public:
    RemoteFileOrFolderRestoredEventHandler(RemoteFileEvent remoteEvent,
        QObject *parent = 0);

protected:
    void run();

private slots:
    void onGetChildrenSucceeded(QList<Drive::RemoteFileDesc>);
    void onGetChildrenFailed();

private:
    FileDownloader *downloader;
    QString localFilePath;
};

class RemoteFileCopiedEventHandler : public RemoteEventHandlerBase
{
    Q_OBJECT
public:
    RemoteFileCopiedEventHandler(RemoteFileEvent remoteEvent,
        QObject *parent = 0);

protected:
    void run();

private slots:
    void onGetAncestorsSucceeded(QString);
    void onGetAncestorsFailed(QString);

private:
    QString localSourcePath;
    QString localTargetPath;

};


}


#endif // REMOTE_EVENT_HANDLERS