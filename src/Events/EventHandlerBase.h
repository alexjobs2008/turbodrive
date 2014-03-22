#ifndef EVENT_HANDLER_BASE
#define EVENT_HANDLER_BASE

#include <QtCore/QThread>
#include <QtCore/QCoreApplication>

namespace Drive
{

struct RemoteFileEvent;
class RemoteFileEventExclusion;
struct LocalFileEvent;
class LocalFileEventExclusion;

class EventHandlerBase : public QThread
{
    Q_OBJECT
public:
    EventHandlerBase(QObject *parent = 0) : QThread(parent) {};
    virtual ~EventHandlerBase() {};

public slots:
    virtual void cancel()
    {
        processEventsAndQuit();
    };

signals:
    void succeeded();
    void failed(const QString& error);

    void newRemoteFileEvent(const RemoteFileEvent& event);
    void newLocalFileEvent(const LocalFileEvent& event);

    void newPriorityRemoteFileEvent(const RemoteFileEvent& event);
    void newPriorityLocalFileEvent(const LocalFileEvent& event);
    
    void newLocalFileEventExclusion(const LocalFileEventExclusion& localExclusion);
    void newRemoteFileEventExclusion(const RemoteFileEventExclusion& remoteExclusion);

protected:
    virtual void processEventsAndQuit()
    {
        QCoreApplication::processEvents();
        quit();
    }
};


}

#endif // EVENT_HANDLER_BASE