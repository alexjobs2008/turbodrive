#ifndef EVENT_HANDLER_BASE
#define EVENT_HANDLER_BASE

#include <QtCore/QPointer>
#include <QtCore/QThread>
#include <QtCore/QCoreApplication>

#include "FileUtils.h"


namespace Drive
{

struct RemoteFileEvent;
class RemoteFileEventExclusion;
class LocalFileEvent;

class LocalFileEventExclusion;

class EventHandlerBase : public QThread
{
    Q_OBJECT

protected:

    // File or folder name handled by this thread.
    // Empty by default.
    QString fileName;
    int state;

public:
	// TODO: remove parent param
	// The object cannot be moved to thread if it has a parent.
	// see also QObject::moveToThread docs.
    EventHandlerBase(QObject*) :
        QThread(nullptr),
        fileName()
	{
        connect(this, &QThread::started, this, &EventHandlerBase::runEventHandlingPrivate, Qt::QueuedConnection);
        connect(this, &EventHandlerBase::quitThread, this, &QThread::quit, Qt::QueuedConnection);

        // moveToThread(this);
    }

    virtual ~EventHandlerBase() { }

    void markSyncing(const QString &fileName);
    void markOk();
    void markError();
    void markDeleted();

signals:

    void setStateSignal(QString& fileName, int state);

    // Mark file deleted and remove it from map
    void setDeletedSignal(QString& fileName);

public:

    void startThread()
	{
		beforeStart();
        connect(this, &QThread::finished, this, &EventHandlerBase::sendFinished, Qt::QueuedConnection);
        connect(this, &QThread::finished, this, &QObject::deleteLater, Qt::QueuedConnection);
        connect(this, &EventHandlerBase::cancel, this, &EventHandlerBase::processEventsAndQuit, Qt::QueuedConnection);
        start();
	}

protected:
    virtual void runEventHandling() = 0;
    virtual void beforeStart() {}

    virtual void processEventsAndQuit()
    {
        Q_EMIT quitThread();
        QCoreApplication::processEvents();
    }

private slots:
    void runEventHandlingPrivate()
    {
        runEventHandling();
    }

    void sendFinished()
    {
        emit finished(this);
    }

signals:
    void finished(EventHandlerBase *handler);

	void succeeded();
    void failed(EventHandlerBase *handler, const QString& error);

	void newRemoteFileEvent(const RemoteFileEvent& event);
	void newLocalFileEvent(const LocalFileEvent& event);

	void newPriorityRemoteFileEvent(const RemoteFileEvent& event);
	void newPriorityLocalFileEvent(const LocalFileEvent& event);

	void newLocalFileEventExclusion(const LocalFileEventExclusion& localExclusion);
	void newRemoteFileEventExclusion(const RemoteFileEventExclusion& remoteExclusion);

    void cancel();
    void quitThread();

};

}

#endif // EVENT_HANDLER_BASE
