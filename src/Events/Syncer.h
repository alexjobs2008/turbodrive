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
    void newRemoteEvent(RemoteFileEvent event);
    void newLocalEvent(LocalFileEvent event);

    void newFileDesc(Drive::RemoteFileDesc fileDesc);

private slots:
    void onGetChildrenSucceeded(QList<Drive::RemoteFileDesc>);
    void onGetChildrenFailed();

private:
    void syncLocalFolder(const QString& localFolderPath);
    void fireEvents();

    QString currentLocalPathPrefix;
    int folderCounter;

    QList<LocalFileEvent> localEvents;
    QList<RemoteFileEvent> remoteEvents;
};

}


#endif // SYNCER_H