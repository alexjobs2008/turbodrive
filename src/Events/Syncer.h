#ifndef SYNCER_H
#define SYNCER_H

#include "APIClient/ApiTypes.h"
#include "Events/LocalFileEvent.h"
#include "watchdog.h"

#include <QtCore/QObject>


namespace Drive
{

class Syncer: public QObject
{
	Q_OBJECT
public:
	Syncer();

	void fullSync();

signals:
	void newRoot(const RemoteFileDesc&);
	void newFile(const RemoteFileDesc&);

	void newRemoteEvent(RemoteFileEvent event);
	void newLocalEvent(LocalFileEvent event);

private:
	void getRoots();
	void onGetRootsSucceeded(const QList<RemoteFileDesc>&);

	void getChildren();
	void onGetChildrenSucceeded(const QList<Drive::RemoteFileDesc>&);

	void onGetFailed() const;

	void syncLocalFolder(const QString& localFolderPath);
	void fireEvents();

private:
	QString m_currentLocalPathPrefix;
	int m_folderCounter;

	QList<LocalFileEvent> m_localEvents;
	QList<RemoteFileEvent> m_remoteEvents;

	WatchDog m_watchDog;
};

}

#endif // SYNCER_H
