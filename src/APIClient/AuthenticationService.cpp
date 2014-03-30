#include "AuthenticationService.h"

#include "Settings/settings.h"
#include "QsLog/QsLog.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace Drive
{

namespace
{


QByteArray toByteArray(const AuthRestResource::Input& data)
{
	static const QString username("username");
	static const QString password("password");

	QVariantMap map;
	map.insert(username, data.username);
	map.insert(password, data.password);

	const QJsonObject jobject = QJsonObject::fromVariantMap(map);

	const QJsonDocument doc(jobject);

	const QString s(QLatin1String("data=") + doc.toJson());

	return s.toLatin1();
}


}

AuthRestResourceRef AuthRestResource::create()
{
	AuthRestResourceRef resource = RestResource::create<AuthRestResource>();
	return resource;
}

void AuthRestResource::login(const AuthRestResource::Input& data)
{
	QLOG_INFO() << "signing in...";

	m_data = data;

	HeaderList headers;
	doOperation(QNetworkAccessManager::PostOperation, toByteArray(data), headers);
}

QString AuthRestResource::path() const
{
	return "/api/v1/login";
}

QString AuthRestResource::service() const
{
	return "AuthService";
}

bool AuthRestResource::restricted() const
{
	return false;
}

bool AuthRestResource::processPostResponse(int status,
										const QByteArray& data,
										const HeaderList& headers)
{
	Q_UNUSED(headers);

	if (status == 0)
	{
		emit loginFailed(tr("Please check your Internet connection"));
		return true;
	}

	if(status == 200)
	{
		const QString& token = getDataFromJson(data);

		if (!token.isEmpty())
		{
			QLOG_INFO() << "Token received: " << token;
			emit loginSucceeded(m_data.username, m_data.password, token);
		}
		else
		{
			QLOG_INFO() << "Token not found in: " << data;
			emit loginFailed(tr("No authentication token found"));
		}
	}
	else
	{
		QString errorString = getDataFromJson(data);
		QLOG_INFO() << "Login failed: " << errorString << ". HTTP status: " << status << ". Data: " << data;
		emit loginFailed(errorString);
	}

	return true;
}

}
