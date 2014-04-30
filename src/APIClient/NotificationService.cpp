#include "NotificationService.h"

#include "Application/AppController.h"
#include "APIClient/ApiTypes.h"

#include "QsLog/QsLog.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

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
	HeaderList headers;
	ParamList params;
	QByteArray idValue = AppController::instance().serviceChannel().toLatin1();

	if (!lastEventTimestamp.isEmpty())
	{
		idValue.prepend(':').prepend(lastEventTimestamp.toLatin1());
	}

	params.append(ParamPair("identifier", idValue));
	doOperation(QNetworkAccessManager::GetOperation, params, headers);
}

QString NotificationResource::path() const
{
	return "/";
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
	if (status != 200)
	{
		QLOG_ERROR() << "NotificationResource error: " << status;
		return true;
	}

	if (data.trimmed().isEmpty())
	{
		listenRemoteFileEvents();
		return true;
	}

	QJsonDocument doc = QJsonDocument::fromJson(data);
	if (!doc.isArray())
	{
		listenRemoteFileEvents();
		return true;
	}

	QJsonArray array = doc.array();

	for (int i = 0; i < array.size(); i++)
	{
		QJsonValue value = array.at(i);
		if (value.type() == QJsonValue::Object)
		{
			QJsonObject obj = value.toObject();
			RemoteFileEvent remoteEvent(RemoteFileEvent::fromJson(obj));

			remoteEvent.log();

			if (remoteEvent.isValid())
			{
				emit newRemoteFileEvent(remoteEvent);
				lastEventTimestamp = remoteEvent.timestamp;
			}
			else
			{
				QLOG_ERROR() << "NotificationResource: remote event is not valid";
			}
		}
	}

	listenRemoteFileEvents();

	return true;
}

}
