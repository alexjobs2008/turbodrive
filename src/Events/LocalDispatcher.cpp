#include "LocalDispatcher.h"

#include "Settings/settings.h"
#include "QsLog/QsLog.h"
#include <iostream>

namespace Drive
{

LocalFileEventDispatcher::LocalFileEventDispatcher(QObject *parent)
	: QObject(parent)
{
	listener = new LocalListener(this);
	
	watchID = fileWatcher.addWatch(
		Settings::instance().get(Settings::folderPath).toString().toStdString(), listener, true);	
}




LocalListener::LocalListener(QObject *parent)
	: QObject(parent)
	, efsw::FileWatchListener()
{
}

void LocalListener::handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename)
{
 	QString sDir = QString::fromStdString(dir);
 	QString sFileName = QString::fromStdString(filename);
	QString sOldFileName = QString::fromStdString(oldFilename);
	
	switch( action )
	{
	case efsw::Actions::Add:
		QLOG_INFO() << "DIR (" << sDir << ") FILE (" << sFileName << ") has event Added";
		break;
	case efsw::Actions::Delete:
		QLOG_INFO() << "DIR (" << sDir << ") FILE (" << sFileName << ") has event Delete";
		break;
	case efsw::Actions::Modified:
		QLOG_INFO() << "DIR (" << sDir << ") FILE (" << sFileName << ") has event Modified";
		break;
	case efsw::Actions::Moved:
		QLOG_INFO() << "DIR (" << sDir << ") FILE (" << sFileName << ") has event Moved from (" << sOldFileName << ")";
		break;
	default:
		QLOG_ERROR() << "LocalFileEventDispatcher event unknown!";
	}
}

}