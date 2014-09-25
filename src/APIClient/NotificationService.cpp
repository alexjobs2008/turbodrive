#include "NotificationService.h"

#include "Application/AppController.h"
#include "APIClient/ApiTypes.h"

#include "QsLog/QsLog.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

using namespace std;

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
	string idValue = AppController::instance().serviceChannel().toStdString();
	if (!lastEventTimestamp.toStdString().empty())
	{
		idValue = lastEventTimestamp.toStdString() + ":" + idValue;
	}

	ParamList params;
	params.append(ParamPair("identifier", QByteArray(idValue.c_str(), idValue.size())));
	doOperation(QNetworkAccessManager::GetOperation, params, HeaderList());
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
		QLOG_ERROR()
				<< "[NotificationResource]: Received data is not a JSON array.";
		listenRemoteFileEvents();
		return true;
	}

	QJsonArray array = doc.array();

    QLOG_TRACE() << "NotificationResource::processGetResponse(): BEGIN logging JSON response: data.size = " << data.size();

    for (int i = 0; i < headers.size(); i++)
    {
        const HeaderPair& pair = headers.at(i);
        QLOG_TRACE() << "Header[" << i << "] = { " << pair.first << ", " << pair.second << " }";
    }

	for (int i = 0; i < array.size(); i++)
	{
		QJsonValue value = array.at(i);

        QLOG_TRACE() << "[" << i << "] = " << value;

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

    QLOG_TRACE() << "NotificationResource::processGetResponse(): END logging JSON response:";

    listenRemoteFileEvents();

	return true;
}

}
