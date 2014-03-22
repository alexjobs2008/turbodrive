#ifndef LOCAL_EVENT_HANDLERS
#define LOCAL_EVENT_HANDLERS

#include "EventHandlerBase.h"
#include "LocalFileEvent.h"
#include "APIClient/FilesService.h"

namespace Drive
{

class LocalEventHandlerBase : public EventHandlerBase
{
	Q_OBJECT
public:
	LocalEventHandlerBase(LocalFileEvent localEvent,
		QObject *parent = 0);

	virtual ~LocalEventHandlerBase();

protected:
	bool localFileExists();

	LocalFileEvent localEvent;
};

class LocalFileOrFolderAddedEventHandler : public LocalEventHandlerBase
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

	void onGetFileObjectSucceeded(RemoteFileDesc fileDesc);
	void onGetFileObjectFailed(const QString& error);

	void onGetFileObjectParentIdSucceeded(int id);
	void onGetFileObjectParentIdFailed();

	void onCreateFolderSucceeded(Drive::RemoteFileDesc fileDesc);
	void onCreateFolderFailed(const QString& error);

	void onUploadSucceeded(Drive::RemoteFileDesc);
	void onUploadFailed(const QString& error);

	void onTrashSucceeded();
	void onTrashFailed(const QString& error);

	void onRemoveSucceeded();
	void onRemoveFailed(const QString& error);

private:
	QString remotePath;
	RemoteFileDesc remotefileDesc;
	GetChildrenResourceRef getChildrenResource;

	int objParentId; // cached version only
};

class LocalFileOrFolderDeletedEventHandler: public LocalEventHandlerBase
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
	int remoteFileObjectId;
};

class LocalFileOrFolderRenamedEventHandler: public LocalEventHandlerBase
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
	int remoteFileObjectId;
	QString newName;
	GetChildrenResourceRef getChildrenResource;
};


}


#endif // LOCAL_EVENT_HANDLERS