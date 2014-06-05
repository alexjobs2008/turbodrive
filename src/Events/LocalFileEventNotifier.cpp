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

void LocalFileEventNotifier::resetFolder()
{
	m_listener.reset(new LocalListener(this));
	connect(m_listener.get(), &LocalListener::newLocalFileEvent,
		this, &LocalFileEventNotifier::newLocalFileEvent);

	const QString dirPath = Settings::instance().get(Settings::folderPath).toString();
	const std::string stdDirPath = dirPath.toLocal8Bit().constData();

	m_watchID = m_fileWatcher.addWatch(stdDirPath, m_listener.get(), true);
	QLOG_DEBUG() << "Directory watcher initialized (" << m_watchID << ").";
	Q_ASSERT(m_watchID > 0);
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
	if (event.type() == LocalFileEvent::Modified)
	{
		// Modified event for local dir should be ignored
		// because we will consider events for each file in dir.
		const QFileInfo fileInfo(event.localPath());
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

void LocalListener::handleFileAction(efsw::WatchID,
									const std::string& dir,
									const std::string& filename,
									efsw::Action action,
									std::string oldFilename)
{
	LocalFileEvent::Type type = LocalFileEvent::Added;
	switch(action)
	{
	case efsw::Actions::Add:
		type = LocalFileEvent::Added;
		break;
	case efsw::Actions::Delete:
		type = LocalFileEvent::Deleted;
		break;
	case efsw::Actions::Modified:
		type = LocalFileEvent::Modified;
		break;
	case efsw::Actions::Moved:
		type = LocalFileEvent::Moved;
		break;
	default:
		Q_ASSERT(false);
		return;
	}

	const LocalFileEvent localEvent(type,
			QDir::cleanPath(QString::fromLocal8Bit(dir.c_str())),
			QDir::cleanPath(QString::fromUtf8(filename.c_str())),
			QDir::cleanPath(QString::fromUtf8(oldFilename.c_str())));

	if (eventShouldBeIgnored(localEvent))
	{
		return;
	}

	emit newLocalFileEvent(localEvent);
}

}
