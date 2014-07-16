#ifndef NOTIFICATION_SERVICE
#define NOTIFICATION_SERVICE

#include "Network/RestResource.h"
#include "watchdog.h"

#define NOTIFICATION_SERVICE_NAME "NotificationService"

namespace Drive
{

struct RemoteFileEvent;

class NotificationResource;
typedef QSharedPointer<NotificationResource> NotificationResourceRef;

class NotificationResource : public RestResource
{
	Q_OBJECT
public:

	static NotificationResourceRef create();

	NotificationResource();

	void listenRemoteFileEvents();

	virtual QString path() const;
	virtual QString service() const;
	virtual bool restricted() const;

signals:
	void newRemoteFileEvent(const RemoteFileEvent& event);

private:
	virtual bool processGetResponse(
			int status, const QByteArray& data, const HeaderList&);

	QString lastEventTimestamp;
	WatchDog m_watchDog;
};

}


#endif // NOTIFICATION_SERVICE
