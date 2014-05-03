#include "LocalFileEventNotifier.h"

#include <iostream>
#include <QtCore/QDir>
#include <QtCore/QDateTime>

#include "QsLog/QsLog.h"

#include "LocalFileEvent.h"
#include "Settings/settings.h"


namespace Drive
{


LocalFileEventNotifier& LocalFileEventNotifier::instance()
{
	static LocalFileEventNotifier singleton;
	return singleton;
}

LocalFileEventNotifier::LocalFileEventNotifier(QObject *parent)
	: QObject(parent)
	, m_listener(nullptr)
{
}

void LocalFileEventNotifier::setFolder()
{
	m_listener.reset(new LocalListener(this));
	connect(m_listener.get(), &LocalListener::newLocalFileEvent,
		this, &LocalFileEventNotifier::newLocalFileEvent);

	m_watchID = m_fileWatcher.addWatch(Settings::instance()
		.get(Settings::folderPath).toString().toStdString(), m_listener.get(), true);
}

void LocalFileEventNotifier::stop()
{
	m_fileWatcher.removeWatch(m_watchID);
}

// ============================================================================


namespace
{

bool eventShouldBeIgnored(const LocalFileEvent& event)
{
	bool result = false;
	if (event.type == LocalFileEvent::Modified)
	{
		// Modified event for local dir should be ignored
		// because we will consider events for each file in dir.

		const QString localPath = QDir::toNativeSeparators(event.dir)
				.append(QDir::toNativeSeparators(event.filePath));

		const QFileInfo fileInfo(localPath);
		result = fileInfo.exists() && fileInfo.isDir();
	}
	return result;
}

}

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

	if (eventShouldBeIgnored(localEvent))
	{
		return;
	}

	emit newLocalFileEvent(localEvent);
}

}
