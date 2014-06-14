#include "DashboardService.h"

#include "QsLog/QsLog.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUrlQuery>


namespace Drive
{

RegisterLinkResourceRef RegisterLinkResource::create()
{
	RegisterLinkResourceRef resource =
		RestResource::create<RegisterLinkResource>();
	return resource;
}

void RegisterLinkResource::requestRegisterLink()
{
	QLOG_INFO() << "Requesting Sign Up link...";

	HeaderList headers;
	QByteArray data;
	doOperation(QNetworkAccessManager::GetOperation, data, headers);
}

QString RegisterLinkResource::path() const
{
	return "/api/v1/registerLink";
}

QString RegisterLinkResource::service() const
{
	return DASHBOARD_SERVICE_NAME;
}

bool RegisterLinkResource::restricted() const
{
	return false;
}

bool RegisterLinkResource::processGetResponse(int status,
											const QByteArray& data,
											const HeaderList&)
{
	QString link = getDataFromJson(data);
	if (status == 200 && !link.isEmpty())
	{
		QLOG_INFO() << "Register link received: " << link;
		emit linkReceived(link);
	}
	else
	{
		QLOG_INFO() << "Error: register link is not received.";
	}

	return true;
}

// Password reset ------------------------------------------------------------

PasswordResetResourceRef PasswordResetResource::create()
{
	PasswordResetResourceRef resource =
		RestResource::create<PasswordResetResource>();
	return resource;
}

void PasswordResetResource::resetPassword(const QString& username)
{
	QLOG_INFO() << "Requesting password reset...";

	HeaderList headers;
//	headers.append(HeaderPair(contentTypeHeader,
//		QString("application/x-www-form-urlencoded").toLatin1()));

	ParamList params;
	params.append(ParamPair("username", username.toLatin1()));

	doOperation(QNetworkAccessManager::PostOperation, params, "data", headers);
}

QString PasswordResetResource::path() const
{
	return "/api/v1/user/password-reset";
}

QString PasswordResetResource::service() const
{
	return DASHBOARD_SERVICE_NAME;
}

bool PasswordResetResource::restricted() const
{
	return false;
}

bool PasswordResetResource::processPostResponse(int status,
												const QByteArray&,
												const HeaderList&)
{
//	QLOG_INFO() << status;
//	QLOG_INFO() << data;
//	QLOG_INFO() << headers;

	switch (status)
	{
	case 200:
		emit resetSuccessfully();
		break;
	case 403:
		emit resetFailed(tr("Password reset failed: too many attempts, please try again later."));
		break;
	case 404:
		emit resetFailed(tr("Password reset failed: user not found."));
		break;
	case 400:
	default:
		emit resetFailed(tr("Password reset failed."));
	}

	return true;
}

QByteArray PasswordResetResource::toByteArray(const QString& email)
{
	// deprecated:
	//	QByteArray md5email =
	//		QCryptographicHash::hash(email.toLatin1(), QCryptographicHash::Md5);

	QVariantMap map;
	map.insert("email", email);
	//map.insert("secret", md5email.toHex());

	QJsonObject jobject = QJsonObject::fromVariantMap(map);
	QJsonDocument doc(jobject);

	QUrlQuery urlQuery;
	urlQuery.addQueryItem("data", doc.toJson());

	//QLOG_INFO() << urlQuery.toString();

	return urlQuery.toString(QUrl::FullyEncoded).toLatin1();
}

}
