#ifndef LOCAL_DISPATCHER
#define LOCAL_DISPATCHER

#include <QtCore/QObject>

#include <iostream>

#include "3rdparty/efsw/include/efsw/efsw.h"
#include "3rdparty/efsw/include/efsw/efsw.hpp"

namespace Drive
{

class LocalListener : public QObject, public efsw::FileWatchListener
{
    Q_OBJECT
public:
	LocalListener(QObject *parent = 0);

	void handleFileAction(efsw::WatchID watchid, const std::string& dir,
		const std::string& filename, efsw::Action action, std::string oldFilename = "" );
};


class LocalFileEventDispatcher : public QObject
{
	Q_OBJECT
public:
	LocalFileEventDispatcher(QObject *parent = 0);

private:
	efsw::FileWatcher fileWatcher;
	LocalListener *listener;
	efsw::WatchID watchID;
};


}

#endif LOCAL_DISPATCHER
