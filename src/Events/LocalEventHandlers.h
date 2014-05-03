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
	LocalFileEvent localEvent;
};

class LocalFileOrFolderAddedEventHandler : public LocalEventHandlerBase
{
	Q_OBJECT
public:
	LocalFileOrFolderAddedEventHandler(LocalFileEvent localEvent,
		QObject *parent = 0);

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
	const QString m_remotePath;
	RemoteFileDesc m_remoteFileDesc;
	GetChildrenResourceRef m_getChildrenResource;
	int m_parentId;
};

class LocalFileOrFolderDeletedEventHandler: public LocalEventHandlerBase
{
	Q_OBJECT
public:
	LocalFileOrFolderDeletedEventHandler(LocalFileEvent localEvent,
		QObject *parent = 0);

protected:
	virtual void run() override;

private slots:
	void onGetFileObjectIdSucceeded(int id);
	void onGetFileObjectIdFailed();
	void onTrashSucceeded();
	void onTrashFailed(const QString& error);

private:
	int m_remoteFileObjectId;
	RestResourceRef m_currentResource;
};

class LocalFileOrFolderRenamedEventHandler: public LocalEventHandlerBase
{
	Q_OBJECT
public:
	LocalFileOrFolderRenamedEventHandler(LocalFileEvent localEvent,
		QObject *parent = 0);

protected:
	void run();

private slots:
	void onGetFileObjectIdSucceeded(int id);
	void onGetFileObjectIdFailed();
	void onRenameSucceeded();
	void onRenameFailed(const QString& error);

private:
	int m_remoteFileObjectId;
	QString m_newName;
	GetChildrenResourceRef m_currentResource;
};


}


#endif // LOCAL_EVENT_HANDLERS
