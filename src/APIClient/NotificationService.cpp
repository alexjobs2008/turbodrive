#include "NotificationService.h"

#include "Application/AppController.h"
#include "QsLog/QsLog.h"

namespace Drive
{

NotificationResourceRef NotificationResource::create()
{
    NotificationResourceRef resource =
        RestResource::create<NotificationResource>();
    return resource;
}

void NotificationResource::listenRemoteFileEvents()
{
	QLOG_INFO() << "Listening for remote file events...";

 	HeaderList headers;
// 	HeaderPair hp(RestResource::referer, "http://rpl.disk.new.assistent.by/?identifier=IFRAME&HOST=disk.new.assistent.by&version=1.32");
// 
// 	headers.append(hp);

	QByteArray data;
	doOperation(QNetworkAccessManager::GetOperation, data, headers);
}

QString NotificationResource::path() const
{
    return QString("/?identifier=th_%1").arg(AppController::instance().profileData().serviceChannel);
}

QString NotificationResource::service() const
{
    return NOTIFICATION_SERVICE_NAME;
}

bool NotificationResource::restricted() const
{
    return false;
}

bool NotificationResource::processGetResponse(int status,
                                              const QByteArray& data,
                                              const HeaderList& headers)
{
    QLOG_TRACE() << data;

	listenRemoteFileEvents();
	
// 	QString link = getDataFromJson(data);
//     if (!link.isEmpty())
//     {
//         QLOG_INFO() << "Register link received: " << link;
//         emit linkReceived(link);
//     }
//     else
//     {
//         QLOG_INFO() << "Error: register link is not received.";
//     }

    return true;
}

}