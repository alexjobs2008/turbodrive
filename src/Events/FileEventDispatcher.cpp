#include "FileEventDispatcher.h"

#include "LocalFileEventNotifier.h"

#include "RemoteEventHandlers.h"
#include "LocalEventHandlers.h"

#include "QsLog/QsLog.h"

#include <QtCore/QThread>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

#define DATETIME_TO_STRING_FORMAT "yyyy-MM-dd hh:mm:ss"

namespace Drive
{

FileEventDispatcher& FileEventDispatcher::instance()
{
	static FileEventDispatcher myself;
	return myself;
}

FileEventDispatcher::FileEventDispatcher(QObject *parent)
	: QObject(parent)
	, state(Finished)
	, currentPosition(0)
	, totalCount(0)
	, globalCounter(0)
	, dontIncrementTotalCount(false)
	, dontIncrementCurrentPosition(false)
{
	eventLogFile = new QFile(this);

	QString dirPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	QDir dir;
	dir.mkpath(dirPath);

	eventLogFile->setFileName(
		QDir(dirPath).filePath("eventLog.txt"));

//	eventLogFile->setFileName(
//		QDir(QCoreApplication::applicationDirPath()).filePath("eventLog.txt"));

	eventLogFile->remove();
	eventLogFile->open(QIODevice::WriteOnly | QIODevice::Text);
}

FileEventDispatcher::~FileEventDispatcher()
{
	cancelAll();
}

void FileEventDispatcher
	::addRemoteFileEvent(Drive::RemoteFileEvent remoteEvent)
{
	logEvent(remoteEvent, "REMOTE event: ");

	if (remoteFileEventShouldBeIgnored(remoteEvent))
	{
		logIgnored();
	}
	else
	{
		remoteEvents.enqueue(remoteEvent);
		proceed();
	}
}

void FileEventDispatcher::addLocalFileEvent(Drive::LocalFileEvent localEvent)
{
	logEvent(localEvent, "LOCAL event: ");

	if (localFileEventShouldBeIgnored(localEvent))
	{
		logIgnored();
	}
	else
	if (shouldBeGrouped(localEvent))
	{
		logGrouped();
	}
	else
	{
		localEvents.enqueue(localEvent);
		proceed();
	}
}

void FileEventDispatcher
	::addPriorityRemoteFileEvent(Drive::RemoteFileEvent remoteEvent)
{
	logEvent(remoteEvent, "REMOTE priority event: ");

	if (remoteFileEventShouldBeIgnored(remoteEvent))
	{
		logIgnored();
	}
	else
	{
		priorityRemoteEvents.enqueue(remoteEvent);
		proceed();
	}
}

void FileEventDispatcher
	::addPriorityLocalFileEvent(Drive::LocalFileEvent localEvent)
{
	logEvent(localEvent, "LOCAL priority event: ");

	if (localFileEventShouldBeIgnored(localEvent))
	{
		logIgnored();
	}
	else
	{
		priorityLocalEvents.enqueue(localEvent);
		proceed();
	}
}

void FileEventDispatcher::start()
{
	if (state != Processing)
	{
		state = Processing;
		next();
	}
}

void FileEventDispatcher::proceed()
{
	if (dontIncrementTotalCount)
	{
		dontIncrementTotalCount = false;
	}
	else
	{
		totalCount++;
		processProgress();
	}

	if (state == Finished)
	{
		next();
	}
}

void FileEventDispatcher::finish()
{
	currentPosition = 0;
	totalCount = 0;
	dontIncrementTotalCount = false;
	dontIncrementCurrentPosition = false;
	state = Finished;
	emit finished();
}

void FileEventDispatcher::next()
{
	if (state == Paused)
	{
		return;
	}

	if (priorityLocalEvents.isEmpty()
		&& priorityRemoteEvents.isEmpty()
		&& remoteEvents.isEmpty()
		&& localEvents.isEmpty())
	{
		finish();
		return;
	}

	state = Processing;
	emit processing();

	if (!dontIncrementCurrentPosition)
	{
		currentPosition++;
		processProgress();
	}
	else
	{
		dontIncrementCurrentPosition = false;
	}

	// High priority queues

	if (!priorityRemoteEvents.isEmpty() && !priorityLocalEvents.isEmpty())
	{
		// both queues are not empty, handle the earliest event

		const LocalFileEvent& localEvent = priorityLocalEvents.head();
		const RemoteFileEvent& remoteEvent = priorityRemoteEvents.head();

		if (localEvent.timeStamp() < remoteEvent.unixtime)
		{
			handleEvent(priorityLocalEvents.dequeue());
		}
		else
		{
			handleEvent(priorityRemoteEvents.dequeue());
		}
		return;
	}
	else if (!priorityLocalEvents.isEmpty())
	{
		handleEvent(priorityLocalEvents.dequeue());
		return;
	}
	else if (!priorityRemoteEvents.isEmpty())
	{
		handleEvent(priorityRemoteEvents.dequeue());
		return;
	}

	// Normal priority queues

	if (!remoteEvents.isEmpty() && !localEvents.isEmpty())
	{
		// both queues are not empty, handle the earliest event

		const LocalFileEvent& localEvent = localEvents.head();
		const RemoteFileEvent& remoteEvent = remoteEvents.head();

		if (localEvent.timeStamp() < remoteEvent.unixtime)
		{
			handleEvent(localEvents.dequeue());
		}
		else
		{
			handleEvent(remoteEvents.dequeue());
		}
	}
	else if (!localEvents.isEmpty())
	{
		handleEvent(localEvents.dequeue());
	}
	else if (!remoteEvents.isEmpty())
	{
		handleEvent(remoteEvents.dequeue());
	}
}

void FileEventDispatcher::handleEvent(const RemoteFileEvent& remoteEvent)
{
	dontIncrementTotalCount = false;

	if (remoteFileEventShouldBeIgnored(remoteEvent))
	{
		logEvent(remoteEvent, "Previously queued REMOTE event: ");
		logIgnored();
		next();
		return;
	}

	// first check if the event in remoteFileEventExclusions

	// UPDATE: exclusion check moved to eventAdded handlers

//	RemoteFileEventExclusion exclusion;
//	exclusion.id = remoteEvent.fileDesc.id;
//	exclusion.type = remoteEvent.type;
//
//	int index = remoteFileEventExclusions.indexOf(exclusion);
//
//	if (index != -1)
//	{
//		QLOG_TRACE() << this << "Remote file event exclusion found for the current event, skipping it.";
//		remoteFileEventExclusions.removeAt(index);
//		next();
//		return;
//	}

	EventHandlerBase *handlerThread = nullptr;

	switch (remoteEvent.type)
	{
	case RemoteFileEvent::Created:
		handlerThread = new RemoteFolderCreatedEventHandler(remoteEvent);
		break;
	case RemoteFileEvent::Uploaded:
		handlerThread = new RemoteFileUploadedEventHandler(remoteEvent);
		break;
	case RemoteFileEvent::Restored:
		handlerThread = new RemoteFileOrFolderRestoredEventHandler(remoteEvent);
		dontIncrementTotalCount = true;
		dontIncrementCurrentPosition = true;
		break;
	case RemoteFileEvent::Renamed:
	case RemoteFileEvent::Moved:
		handlerThread = new RemoteFileRenamedEventHandler(remoteEvent);
		break;
	case RemoteFileEvent::Trashed:
		handlerThread = new RemoteFileTrashedEventHandler(remoteEvent);
		break;
	case RemoteFileEvent::Copied:
		//handlerThread = new RemoteFileCopiedEventHandler(remoteEvent);
		break;
	default:
		break;
	}

	startHandlerThreadOrProcessNext(handlerThread);
}

void FileEventDispatcher::handleEvent(const LocalFileEvent& localEvent)
{
	QLOG_TRACE() << "FileEventDispatcher::handleEvent CURRENT POS:"
		<< currentPosition;

	dontIncrementTotalCount = false;

	QString fileName = localEvent.localPath().
		split(QDir::separator(), QString::SkipEmptyParts).last();

	if (!fileName.isEmpty())
	{
		if (fileName.startsWith("~$"))
		{
			QLOG_TRACE() << "Skipping local file event, because of temp file:"
				<< fileName;

			next();
			return;
		}
	}

	EventHandlerBase *handlerThread = 0;

	switch (localEvent.type())
	{
	case LocalFileEvent::Added:
	case LocalFileEvent::Modified:
		handlerThread = new LocalFileOrFolderAddedEventHandler(localEvent);
		break;
	case LocalFileEvent::Deleted:
		handlerThread = new LocalFileOrFolderDeletedEventHandler(localEvent);
		break;
	case LocalFileEvent::Moved:
		handlerThread = new LocalFileOrFolderRenamedEventHandler(localEvent);
		break;
	default :
		break;
	}

	startHandlerThreadOrProcessNext(handlerThread);
}

void FileEventDispatcher::startHandlerThreadOrProcessNext(
	EventHandlerBase* handlerThread)
{
	if (handlerThread)
	{
		connect(handlerThread, &EventHandlerBase::finished,
				this, &FileEventDispatcher::onFinishProcessingEvent);

		connect(handlerThread, &EventHandlerBase::succeeded,
				this, &FileEventDispatcher::onEventHandlerSucceeded);

		connect(handlerThread, &EventHandlerBase::failed,
				this, &FileEventDispatcher::onEventHandlerFailed);

		connect(handlerThread, &EventHandlerBase::newLocalFileEventExclusion,
				this, &FileEventDispatcher::onNewLocalFileEventExclusion,
				Qt::BlockingQueuedConnection);

		connect(handlerThread, &EventHandlerBase::newRemoteFileEventExclusion,
				this, &FileEventDispatcher::onNewRemoteFileEventExclusion,
				Qt::BlockingQueuedConnection);

		connect(handlerThread, &EventHandlerBase::newRemoteFileEvent,
				this, &FileEventDispatcher::addRemoteFileEvent,
				Qt::BlockingQueuedConnection);

		connect(handlerThread, &EventHandlerBase::newLocalFileEvent,
				this, &FileEventDispatcher::addLocalFileEvent,
				Qt::BlockingQueuedConnection);

		connect(handlerThread, &EventHandlerBase::newPriorityRemoteFileEvent,
				this, &FileEventDispatcher::addPriorityRemoteFileEvent,
				Qt::BlockingQueuedConnection);

		connect(handlerThread, &EventHandlerBase::newPriorityLocalFileEvent,
				this, &FileEventDispatcher::addPriorityLocalFileEvent,
				Qt::BlockingQueuedConnection);

		eventHandlers << handlerThread;

		handlerThread->start();
	}
	else
	{
		next();
	}
}

void FileEventDispatcher::pause()
{
	state = Paused;
	emit paused();
}

void FileEventDispatcher::cancelAll()
{
	priorityRemoteEvents.clear();
	priorityLocalEvents.clear();

	remoteEvents.clear();
	localEvents.clear();

	QListIterator<EventHandlerBase*> i(eventHandlers);
	while (i.hasNext())
	{
		EventHandlerBase* eventHandler = i.next();
		eventHandler->cancel();
	}

	finish();
}

void FileEventDispatcher::onEventHandlerSucceeded()
{
	QThread *thread = static_cast<QThread*>(sender());
	QLOG_TRACE() << "Thread" << thread << "successfully handled event.";
}

void FileEventDispatcher::onEventHandlerFailed(const QString& error)
{
	QThread *thread = static_cast<QThread*>(sender());

	QLOG_TRACE() << "Thread" << thread
		<< "failed to handle event:" << error;
}

void FileEventDispatcher::onFinishProcessingEvent()
{
	EventHandlerBase *thread = static_cast<EventHandlerBase*>(sender());

	QLOG_TRACE() << "Thread" << thread << "is finished processing.";
	QLOG_TRACE() << "Removing local file event exclusions for " << thread;

	localFileEventExclusions.remove(thread);
	eventHandlers.removeOne(thread);

	thread->deleteLater();
	next();
}

void FileEventDispatcher::onNewLocalFileEventExclusion(const LocalFileEventExclusion &localExclusion)
{
	QMutexLocker locker(&localExclusionsMutex);

	EventHandlerBase *thread = static_cast<EventHandlerBase*>(sender());

	LocalFileEventExclusionList threadLocalExclusions =
		localFileEventExclusions.value(thread, LocalFileEventExclusionList());

	threadLocalExclusions << localExclusion;
	localFileEventExclusions.insert(thread, threadLocalExclusions);
}

bool FileEventDispatcher::localFileEventShouldBeIgnored(const LocalFileEvent &event)
{
	QMapIterator<EventHandlerBase*, LocalFileEventExclusionList> i(localFileEventExclusions);
	while (i.hasNext())
	{
		i.next();
		LocalFileEventExclusionList exclusions = i.value();
		foreach (LocalFileEventExclusion localFileEventExclusion, exclusions)
		{
			if (localFileEventExclusion.matches(event))
			{
				return true;
			}
		}
	}

	return false;
}

void FileEventDispatcher::onNewRemoteFileEventExclusion(
	const RemoteFileEventExclusion &remoteExclusion)
{
	QMutexLocker locker(&remoteExclusionsMutex);
	remoteExclusion.log();
	remoteFileEventExclusions.append(remoteExclusion);
}

bool FileEventDispatcher::remoteFileEventShouldBeIgnored(
	const RemoteFileEvent &event)
{
	for (int i = 0; i < remoteFileEventExclusions.size(); ++i)
	{
		RemoteFileEventExclusion exclusion = remoteFileEventExclusions.at(i);
		if (exclusion.matches(event))
		{
			remoteFileEventExclusions.removeAt(i);
			return true;
		}
	}

	return false;
}

bool FileEventDispatcher::shouldBeGrouped(const LocalFileEvent &event)
{
	// Grouping events EEs with some another event E means that EEs
	// should be ignored because semantically EEs will be processed
	// by E handler. So executing EEs handlers would be useless
	// ---
	// Grouping rules:
	// 1. not processed "modified" event should be grouped with
	//	a *prior* and not processed "added" event
	// 2. not processed "modified" event should be grouped with
	//	a *prior* and not processed "modified" event

	// 3. not processed "deleted" event should be grouped with
	//	a *latter* "deleted" event.

	// 3old. not processed "added" and "modified" events should be grouped with
	//	a *latter* "deleted" event.
	//	OTOH: user would have not the current version of a file in the trash

	if (event.type() == LocalFileEvent::Modified)
	{
		for (int i = 0; i < localEvents.size(); ++i)
		{
			LocalFileEvent queuedEvent(localEvents.at(i));

			// Rules 1 and 2:

			if ( (queuedEvent.type() == LocalFileEvent::Added
				|| queuedEvent.type() == LocalFileEvent::Modified)
				&& queuedEvent.localPath() == event.localPath())
			{
				return true;
			}
		}
	}

	return false;
}

QString FileEventDispatcher::stateToString()
{
	switch (state)
	{
	case Drive::FileEventDispatcher::Finished:
		return "Finished";
	case Drive::FileEventDispatcher::Stopped:
		return "Stopped";
	case Drive::FileEventDispatcher::Paused:
		return "Paused";
	case Drive::FileEventDispatcher::Processing:
		return "Processing";
	default:
		Q_ASSERT(false);
		return "N/A";
	}
}

void FileEventDispatcher::processProgress() const
{
	if (state == Processing)
		emit progress(currentPosition, totalCount);
}

int FileEventDispatcher::queuesSize() const
{
	return priorityLocalEvents.size()
		+ priorityRemoteEvents.size()
		+ remoteEvents.size()
		+ localEvents.size();
}

void FileEventDispatcher::logEvent(const RemoteFileEvent &event, const QString &prefix)
{
	QString line = QString::number(++globalCounter)
		.append("). ")
		.append(QDateTime::fromTime_t(event.unixtime)
			.toString(DATETIME_TO_STRING_FORMAT))
		.append("   ");

	line
		.append(prefix)
		.append(event.typeName())
		.append(" ")
		.append(event.fileDesc.typeName())
		.append(" \"")
		.append(event.fileDesc.name)
		.append("\", id: ")
		.append(QString::number(event.fileDesc.id))
		.append("\n");

	eventLogFile->write(line.toUtf8());
	eventLogFile->flush();
}

void FileEventDispatcher::logEvent(const LocalFileEvent &event, const QString &prefix)
{
	QString line = QString::number(++globalCounter)
		.append("). ")
		.append(QDateTime::fromTime_t(event.timeStamp())
			.toString(DATETIME_TO_STRING_FORMAT))
		.append("   ");

	line
		.append(prefix)
		.append(event.type())
		.append(" \"")
		.append(event.localPath())
		.append("\"");

	line.append("\n");

	eventLogFile->write(line.toUtf8());
	eventLogFile->flush();
}

void FileEventDispatcher::logIgnored() const
{
	QString line("  - IGNORED\n");
	eventLogFile->write(line.toUtf8());
	eventLogFile->flush();
}

void FileEventDispatcher::logGrouped() const
{
	QString line("  - GROUPED with another event\n");
	eventLogFile->write(line.toUtf8());
	eventLogFile->flush();
}

}
