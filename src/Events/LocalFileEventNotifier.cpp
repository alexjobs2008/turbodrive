#include "LocalFileEventNotifier.h"
#include "LocalFileEvent.h"

#include "Settings/settings.h"
#include "QsLog/QsLog.h"

#include <QtCore/QDir>
#include <QtCore/QDateTime>

#include <iostream>


namespace Drive
{


LocalFileEventNotifier& LocalFileEventNotifier::instance()
{
	static LocalFileEventNotifier myself;
	return myself;
}

LocalFileEventNotifier::LocalFileEventNotifier(QObject *parent)
	: QObject(parent)
	, listener(0)
{
}

LocalFileEventNotifier::~LocalFileEventNotifier()
{
}

void LocalFileEventNotifier::setFolder()
{
	if (listener)
		delete listener;

	exclusions.clear();

	listener = new LocalListener(this);

	connect(listener, SIGNAL(newLocalFileEvent(LocalFileEvent)),
		this, SLOT(onNewLocalFileEvent(LocalFileEvent)));

	watchID = fileWatcher.addWatch(Settings::instance()
		.get(Settings::folderPath).toString().toStdString(), listener, true);
}

void LocalFileEventNotifier::stop()
{
	fileWatcher.removeWatch(watchID);
}

// void LocalFileEventNotifier::addExclusion(const QString& localPath)
// {
//	QLOG_TRACE() << "Adding local file events exclusion:" << localPath;
//	exclusions.insert(localPath);
// }
//
// void LocalFileEventNotifier::removeExclusion(const QString& localPath)
// {
//	QLOG_TRACE() << "Removing local file events exclusion:" << localPath;
//	exclusions.remove(localPath);
// }

void LocalFileEventNotifier::onNewLocalFileEvent(const LocalFileEvent& event)
{
	QString localPath = QDir::toNativeSeparators(event.dir)
		.append(QDir::toNativeSeparators(event.filePath));

	QLOG_TRACE() << "LocalFileEventNotifier: New local file event:";
	event.log();

	// "Folder is modified" local file event should be ignored:
	if (event.type == LocalFileEvent::Modified)
	{
		QFileInfo fileInfo(localPath);
		if (fileInfo.exists())
			if (fileInfo.isDir())
			{
				QLOG_TRACE() << "LocalFileEventNotifier: ignoring the event";
				return;
			}
	}

	emit newLocalFileEvent(event);
}


// ============================================================================

LocalListener::LocalListener(QObject *parent)
	: QObject(parent)
	, efsw::FileWatchListener()
{
}

void LocalListener::handleFileAction(efsw::WatchID watchid,
									const std::string& dir,
									const std::string& filename,
									efsw::Action action,
									std::string oldFilename)
{
	LocalFileEvent localEvent;
	localEvent.dir = QString::fromStdString(dir);
	localEvent.filePath = QString::fromStdString(filename);
	localEvent.oldFileName = QString::fromStdString(oldFilename);

	switch( action )
	{
	case efsw::Actions::Add:
		localEvent.type = LocalFileEvent::Added;
		break;
	case efsw::Actions::Delete:
		localEvent.type = LocalFileEvent::Deleted;
		break;
	case efsw::Actions::Modified:
		localEvent.type = LocalFileEvent::Modified;
		break;
	case efsw::Actions::Moved:
		localEvent.type = LocalFileEvent::Moved;
		break;
	default:
		QLOG_ERROR() << "Unknown local file event type.";
		Q_ASSERT(false);
		return;
	}

	emit newLocalFileEvent(localEvent);
}

}