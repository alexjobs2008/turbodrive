#include "ProfileService.h"

#include "Settings/settings.h"
#include "QsLog/QsLog.h"

#include <QtCore/QCryptographicHash>
#include <QtCore/QUrlQuery>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUrlQuery>
#include <QtCore/QStringList>

#define PROFILE_SERVICE_NAME "ProfileService"

namespace Drive
{

ProfileRestResourceRef ProfileRestResource::create()
{
	ProfileRestResourceRef resource = RestResource::create<ProfileRestResource>();
	return resource;
}

void ProfileRestResource::requestProfileData()
{
	QLOG_INFO() << "Requesting user data...";

	doOperation(QNetworkAccessManager::GetOperation, QByteArray(), HeaderList());
}

QString ProfileRestResource::path() const
{
	return "/api/v1/user";
}

QString ProfileRestResource::service() const
{
	return PROFILE_SERVICE_NAME;
}

bool ProfileRestResource::restricted() const
{
	return true;
}

bool ProfileRestResource::processGetResponse(int,
										const QByteArray& data,
										const HeaderList&)
{
	QLOG_INFO() << "User data: " << data;

	QJsonDocument doc = QJsonDocument::fromJson(data);
	if (!doc.isEmpty() && doc.isObject())
	{
		emit profileDataReceived(doc.object());
		return true;
	}

	QLOG_ERROR() << "User data: no data";
	emit profileDataError();
	return true;
}

}
