#ifndef SYNCER_H
#define SYNCER_H

#include "APIClient/ApiTypes.h"
#include "Events/LocalFileEvent.h"

#include <QtCore/QObject>


namespace Drive
{

class Syncer: public QObject
{
	Q_OBJECT
public:
	Syncer(QObject *parent = 0);

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
	QString currentLocalPathPrefix;
	int folderCounter;

	QList<LocalFileEvent> localEvents;
	QList<RemoteFileEvent> remoteEvents;
};

}

#endif // SYNCER_H
