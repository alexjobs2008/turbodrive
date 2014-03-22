#ifndef LOCAL_DISPATCHER
#define LOCAL_DISPATCHER

#include <QtCore/QObject>
#include <QtCore/QSet>

#include <iostream>

#include "3rdparty/efsw/include/efsw/efsw.h"
#include "3rdparty/efsw/include/efsw/efsw.hpp"

namespace Drive
{

struct LocalFileEvent;
class LocalListener;


class LocalFileEventNotifier: public QObject
{
    Q_OBJECT
public:
    static LocalFileEventNotifier& instance();
    ~LocalFileEventNotifier();

public slots:
    void setFolder();
    void stop();

//  Exclusions logic moved to EventDispatcher
//     void addExclusion(const QString& localPath);
//     void removeExclusion(const QString& localPath);

signals:
    void newLocalFileEvent(const LocalFileEvent& event);

private slots:
    void onNewLocalFileEvent(const LocalFileEvent& event);

private:
    LocalFileEventNotifier(QObject *parent = 0);
    Q_DISABLE_COPY(LocalFileEventNotifier)

    efsw::FileWatcher fileWatcher;
    LocalListener *listener;
    efsw::WatchID watchID;
    QSet<QString> exclusions;
};

class LocalListener : public QObject, public efsw::FileWatchListener
{
    Q_OBJECT
public:
	LocalListener(QObject *parent = 0);

	void handleFileAction(efsw::WatchID watchid, const std::string& dir,
		const std::string& filename, efsw::Action action, std::string oldFilename = "" );

signals:
    void newLocalFileEvent(const LocalFileEvent& localFileEvent);

};





}

#endif LOCAL_DISPATCHER
