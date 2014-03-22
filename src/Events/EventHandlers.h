#ifndef EVENT_TYPES_H
#define EVENT_TYPES_H

#include "APIClient/ApiTypes.h"
#include "APIClient/FilesService.h"
#include "LocalDispatcher.h"

#include <QtCore/QThread>
#include <QtCore/QString>

namespace Drive
{

class RemoteFolderCreatedEventHandler : public QThread
{
    Q_OBJECT
public:
    RemoteFolderCreatedEventHandler(RemoteFileEvent remoteEvent,
        QObject *parent = 0);

signals:
    void failed(const QString& error);

protected:
    void run();

private slots:
    void onGetAncestorsSucceeded(const QString& fullPath);
    void onGetAncestorsFailed();

private:
	RemoteFileEvent remoteEvent;
};

class RemoteFileRenamedEventHandler : public QThread
{
    Q_OBJECT
public:
    RemoteFileRenamedEventHandler(RemoteFileEvent remoteEvent,
        QObject *parent = 0);

signals:
    void failed(const QString& error);

protected:
    void run();

private slots:
    void onGetAncestorsSucceeded(const QString& fullPath);
    void onGetAncestorsFailed();

private:
    RemoteFileEvent remoteEvent;
};

class RemoteFileTrashedEventHandler : public QThread
{
    Q_OBJECT
public:
    RemoteFileTrashedEventHandler(RemoteFileEvent remoteEvent,
        QObject *parent = 0);

signals:
    void failed(const QString& error);

protected:
    void run();

private slots:
    void onGetAncestorsSucceeded(const QString& fullPath);
    void onGetAncestorsFailed();

private:
    RemoteFileEvent remoteEvent;
    QString localPath;
};


class FileDownloader;

class RemoteFileUploadedEventHandler : public QThread
{
    Q_OBJECT
public:
    RemoteFileUploadedEventHandler(RemoteFileEvent remoteEvent,
        QObject *parent = 0);

signals:
    void failed(const QString& error);
    void removeLocalFileEventExclusion(const QString& localPath);
    void newLocalEvent(const LocalFileEvent& event);

protected:
    void run();

private slots:
    void onGetAncestorsSucceeded(const QString& fullPath);
    void onGetAncestorsFailed();
    void onDownloadSucceeded();
    void onDownloadFailed(const QString& error);

private:
    RemoteFileEvent remoteEvent;
    FileDownloader *downloader;
    QString localFilePath;
};

class LocalFileOrFolderAddedEventHandler : public QThread
{
    Q_OBJECT
public:
    LocalFileOrFolderAddedEventHandler(LocalFileEvent localEvent,
        QObject *parent = 0);

signals:
    void failed(const QString& error);

protected:
    void run();

private slots:
    void onGetFileObjectIdSucceeded(int id);
    void onGetFileObjectIdFailed();
    void onCreateFolderSucceeded();
    void onCreateFolderFailed(const QString& error);
    void onUploadSucceeded();
    void onUploadFailed(const QString& error);

private:
    LocalFileEvent localEvent;
};

class LocalFileOrFolderDeletedEventHandler: public QThread
{
    Q_OBJECT
public:
    LocalFileOrFolderDeletedEventHandler(LocalFileEvent localEvent,
        QObject *parent = 0);

signals:
    void failed(const QString& error);

protected:
    void run();

private slots:
    void onGetFileObjectIdSucceeded(int id);
    void onGetFileObjectIdFailed();
    void onTrashSucceeded();
    void onTrashFailed(const QString& error);

private:
    LocalFileEvent localEvent;
};

class LocalFileOrFolderRenamedEventHandler: public QThread
{
    Q_OBJECT
public:
    LocalFileOrFolderRenamedEventHandler(LocalFileEvent localEvent,
        QObject *parent = 0);

signals:
    void failed(const QString& error);

protected:
    void run();

private slots:
    void onGetFileObjectIdSucceeded(int id);
    void onGetFileObjectIdFailed();
    void onRenameSucceeded();
    void onRenameFailed(const QString& error);


private:
    LocalFileEvent localEvent;
    int id;
    QString newName;
    GetChildrenResourceRef getChildrenResource;
};




}


#endif // EVENT_TYPES_H