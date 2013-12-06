#include "AuthenticationService.h"

#include "Settings/settings.h"
#include "QsLog/QsLog.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace Drive
{

const QString AuthRestResource::username("username");
const QString AuthRestResource::password("password");

AuthRestResourceRef AuthRestResource::create()
{
    AuthRestResourceRef resource = RestResource::create<AuthRestResource>();
    return resource;
}

void AuthRestResource::login(const AuthRestResource::Input& data)
{
    QLOG_INFO() << "signing in...";

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
		QString token = getDataFromJson(data);

        if (!token.isEmpty())
		{
			QLOG_INFO() << "Token received: " << token;
			emit loginSucceeded(token);
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

QByteArray AuthRestResource::toByteArray(const Input& data)
{
    QVariantMap map;
    map.insert(username, Settings::instance().get(Settings::email).toString());
    map.insert(password, Settings::instance().get(Settings::password).toString());

    QJsonObject jobject = QJsonObject::fromVariantMap(map);

    QJsonDocument doc(jobject);

    QString s = "data=";
    s += doc.toJson();

    return s.toLatin1();
}

}