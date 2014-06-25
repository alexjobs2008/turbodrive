#ifndef REMOTE_EVENT_HANDLERS
#define REMOTE_EVENT_HANDLERS

#include "EventHandlerBase.h"
#include "APIClient/ApiTypes.h"
#include "Events/Cache.h"

namespace Drive
{

class RemoteEventHandlerBase: public EventHandlerBase
{
	Q_OBJECT

public:
	RemoteEventHandlerBase(RemoteFileEvent remoteEvent, QObject* parent);

private:
	virtual void beforeStart() override
	{
		LocalCache::instance().addFile(m_remoteEvent.fileDesc);
	}

protected:
	const RemoteFileEvent m_remoteEvent;
};

class RemoteFolderCreatedEventHandler : public RemoteEventHandlerBase
{
	Q_OBJECT
public:
	RemoteFolderCreatedEventHandler(RemoteFileEvent m_remoteEvent,
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
	RemoteFileRenamedEventHandler(RemoteFileEvent m_remoteEvent,
		QObject *parent = 0);

protected:
	void run();

private:
	virtual void beforeStart() { }

private slots:
	void onGetAncestorsSucceeded(const QString& fullPath);
	void onGetAncestorsFailed();
};

class RemoteFileTrashedEventHandler : public RemoteEventHandlerBase
{
	Q_OBJECT
public:
	RemoteFileTrashedEventHandler(RemoteFileEvent m_remoteEvent,
		QObject *parent = 0);

protected:
	void run();

private slots:
	void onGetAncestorsSucceeded(const QString& fullPath);
	void onGetAncestorsFailed();

private:
	QString m_localPath;
};


class FileDownloader;

class RemoteFileUploadedEventHandler : public RemoteEventHandlerBase
{
	Q_OBJECT
public:
	RemoteFileUploadedEventHandler(RemoteFileEvent m_remoteEvent,
		QObject *parent = 0);

protected:
	void run();

private slots:
	void onGetAncestorsSucceeded(const QString& fullPath);
	void onGetAncestorsFailed();
	void onDownloadSucceeded();
	void onDownloadFailed(const QString& error);

private:
	FileDownloader *m_downloader;
	QString m_localFilePath;
};

class RemoteFileOrFolderRestoredEventHandler : public RemoteEventHandlerBase
{
	Q_OBJECT
public:
	RemoteFileOrFolderRestoredEventHandler(RemoteFileEvent m_remoteEvent,
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
	RemoteFileCopiedEventHandler(RemoteFileEvent m_remoteEvent,
		QObject *parent = 0);

protected:
	void run();

private slots:
	void onGetAncestorsSucceeded(QString);
	void onGetAncestorsFailed();

private:
	QString localSourcePath;
	QString localTargetPath;

};


}


#endif // REMOTE_EVENT_HANDLERS
