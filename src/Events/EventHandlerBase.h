#ifndef EVENT_HANDLER_BASE
#define EVENT_HANDLER_BASE

#include <QtCore/QPointer>
#include <QtCore/QThread>
#include <QtCore/QCoreApplication>

namespace Drive
{

struct RemoteFileEvent;
class RemoteFileEventExclusion;
class LocalFileEvent;

class LocalFileEventExclusion;

class EventHandlerBase : public QObject
{
	Q_OBJECT
public:
	// TODO: remove parent param
	// The object cannot be moved to thread if it has a parent.
	// see also QObject::moveToThread docs.
	EventHandlerBase(QObject*)
		: QObject(nullptr)
	{
		m_thread = new QThread();

		connect(m_thread.data(), &QThread::started, this, &EventHandlerBase::run);
		connect(m_thread.data(), &QThread::finished, this, &EventHandlerBase::finished);

		connect(this, &EventHandlerBase::quitThread, m_thread.data(), &QThread::quit, Qt::QueuedConnection);

		moveToThread(m_thread.data());
	};

	virtual ~EventHandlerBase() {};

	void start()
	{
		beforeStart();
		m_thread->start();
	}

protected:
	virtual void run() = 0;
	void exec() {}

private:
	virtual void beforeStart() {}

public slots:
	virtual void cancel()
	{
		processEventsAndQuit();
	};

signals:
	void finished();

	void succeeded();
	void failed(const QString& error);

	void newRemoteFileEvent(const RemoteFileEvent& event);
	void newLocalFileEvent(const LocalFileEvent& event);

	void newPriorityRemoteFileEvent(const RemoteFileEvent& event);
	void newPriorityLocalFileEvent(const LocalFileEvent& event);

	void newLocalFileEventExclusion(const LocalFileEventExclusion& localExclusion);
	void newRemoteFileEventExclusion(const RemoteFileEventExclusion& remoteExclusion);

	void quitThread();

protected:
	virtual void processEventsAndQuit()
	{
		QCoreApplication::processEvents();
		Q_EMIT quitThread();
	}

private:
	QPointer<QThread> m_thread;
};

}

#endif // EVENT_HANDLER_BASE
