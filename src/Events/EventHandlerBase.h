#ifndef EVENT_HANDLER_BASE
#define EVENT_HANDLER_BASE

#include <QtCore/QPointer>
#include <QtCore/QThread>
#include <QtCore/QCoreApplication>

namespace Drive
{

struct RemoteFileEvent;
class RemoteFileEventExclusion;
struct LocalFileEvent;
class LocalFileEventExclusion;

class EventHandlerBase : public QObject
{
    Q_OBJECT
public:
    EventHandlerBase(QObject* parent = nullptr)
        : QObject(nullptr)
    {
        // TODO: remove parent param
        // The object cannot be moved to thread if it has a parent.
        // see also QObject::moveToThread docs.
        parent;

        auto thread = new QThread();

        connect(thread, &QThread::started, this, &EventHandlerBase::run);
        connect(thread, &QThread::finished, this, &EventHandlerBase::finished);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        connect(this, &EventHandlerBase::quitThread, thread, &QThread::quit, Qt::BlockingQueuedConnection);

        moveToThread(thread);

        m_thread = thread;
    };

    virtual ~EventHandlerBase() {};

    void start() { m_thread->start(); }

protected:
    virtual void run() = 0;
    void exec() { }

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
