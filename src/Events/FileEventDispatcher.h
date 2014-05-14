#ifndef QUEUE_PROCESSOR_H
#define QUEUE_PROCESSOR_H

#include "APIClient/APITypes.h"
#include "LocalFileEvent.h"

#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QFile>
#include <QtNetwork/QNetworkCookie>

namespace Drive
{

// struct RemoteFileEvent;
// class RemoteFileEventExclusion;
// struct LocalFileEvent;
// class LocalFileEventExclusion;

typedef QList<LocalFileEventExclusion> LocalFileEventExclusionList;

class EventHandlerBase;

class FileEventDispatcher : public QObject
{
	Q_OBJECT
public:

	enum State
	{
		Finished = 0,
		Stopped,
		Paused,
		Processing
	};

	static FileEventDispatcher& instance();

	virtual ~FileEventDispatcher();

	void start();
	void pause();
	void cancelAll();

public slots:
	void addRemoteFileEvent(Drive::RemoteFileEvent remoteEvent);
	void addLocalFileEvent(Drive::LocalFileEvent localEvent);

	void addPriorityRemoteFileEvent(Drive::RemoteFileEvent remoteEvent);
	void addPriorityLocalFileEvent(Drive::LocalFileEvent localEvent);

signals:
	void finished();
	void processing();
	void paused();
	void progress(int position, int totalEvents) const;

private slots:
	void onEventHandlerSucceeded();
	void onEventHandlerFailed(const QString& error);

	void onFinishProcessingEvent();

	void onNewLocalFileEventExclusion(const LocalFileEventExclusion &localExclusion);
	void onNewRemoteFileEventExclusion(const RemoteFileEventExclusion &remoteExclusion);

//	void onNewRemoteEvent(const RemoteFileEvent& event);
//	void onNewLocalEvent(const LocalFileEvent& event);

private:
	explicit FileEventDispatcher(QObject *parent = 0);
	Q_DISABLE_COPY(FileEventDispatcher)

	void proceed();
	void finish();
	void next();
	void handleEvent(const RemoteFileEvent& remoteEvent);
	void handleEvent(const LocalFileEvent& remoteEvent);
	void startHandlerThreadOrProcessNext(EventHandlerBase* handlerThread);

	bool localFileEventShouldBeIgnored(const LocalFileEvent &event);
	bool remoteFileEventShouldBeIgnored(const RemoteFileEvent &event);

	bool shouldBeGrouped(const LocalFileEvent &event);

	void log();
	QString stateToString();
	void processProgress() const;
	int queuesSize() const;
	void logEvent(const RemoteFileEvent &event, const QString &prefix = QString());
	void logEvent(const LocalFileEvent &event, const QString &prefix = QString());

	void logIgnored() const;
	void logGrouped() const;

	State state;

	QQueue<RemoteFileEvent> remoteEvents;
	QQueue<LocalFileEvent> localEvents;

	QQueue<RemoteFileEvent> priorityRemoteEvents;
	QQueue<LocalFileEvent> priorityLocalEvents;

	QList<EventHandlerBase*> eventHandlers;

	LocalFileEventExclusionList localFileEventExclusions;
	QList<RemoteFileEventExclusion> remoteFileEventExclusions;

	QMutex remoteExclusionsMutex;
	QMutex localExclusionsMutex;

	int currentPosition;
	int totalCount;

	int globalCounter;

	bool dontIncrementTotalCount; // if currently processing event is restore
	bool dontIncrementCurrentPosition; // if last processed event was restore
	bool lastSuccessfullyHandled;

	QFile *eventLogFile;
};

}

#endif // QUEUE_PROCESSOR_H
