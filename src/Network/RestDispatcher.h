#ifndef REST_DISPATCHER_H
#define REST_DISPATCHER_H

#include "RestResource.h"

#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QQueue>
#include <QtNetwork/QNetworkCookie>
#include <QtNetwork/QNetworkCookieJar>
#include <QMutex>

#define COOKIE_URL "api.new.assistent.by"
//PHPSESSID=76bb1c45cab8d68c22967fea93ea40eb; expires=Sat, 14 Dec 2013 00:50:26 GMT; path=/; domain=disk.new.assistent.by

class RestNetworkAccessManager;
class RestService;
class QUrl;
class QUrlQuery;
class QNetworkReply;

class GeneralRestDispatcher : public QObject
{
	Q_OBJECT
public:

	enum Mode
	{
		Unauthorized,
		Authorized
	};

	enum ReconnectErrorReason
	{
		InvalidCredentials = 0,
		OperationJustFailed
	};

	static GeneralRestDispatcher& instance();

	void request(const RestResource::RequestRef& request);
	void cancelAll();
	void cancelAll(const RestResourceRef& resource);

	QUrl buildUrl(const QString& serviceName, const QString &path, const RestResource::ParamList &params = RestResource::ParamList()) const;
	QUrl buildUrl(const RestResource* restResource);

	void setAuthToken(const QString& token);
	void setWorkspaceId(int workspaceId);

	QList<QNetworkCookie> getCookies();

	void onServices(const QHash<QString, RestService*>& m_services);

signals:
	void dispatcherAuthorized();
	void dispatcherUnauthorized();
	void dispatcherReconnectFailed(GeneralRestDispatcher::ReconnectErrorReason);
	void proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*);
	void cookiesReceived(const QList<QNetworkCookie> &cookies);

protected:
	void initServices();

private:
	void replyFinished(QNetworkReply* networkReply);
	void onSslErrors(QNetworkReply *reply, const QList<QSslError> & errors);

private:
	explicit GeneralRestDispatcher(QObject *parent = 0);
	Q_DISABLE_COPY(GeneralRestDispatcher)

	void next();
	void setMode(Mode newMode);

	void cancelAll(QQueue<RestResource::RequestRef>& queue,
		const RestResourceRef* restResource = 0);

	void cancelCurrent(RestService* service,
		const RestResourceRef* restResource = 0);

	QNetworkRequest createRequest(RestService* service) const;
	QUrlQuery createParams(RestResource::ParamList &params);

	void doOperation(RestResource::Operation operation, RestService* service);

private:
	Mode mode;
	RestNetworkAccessManager *networkAccessManager;
	QHash<QString, RestService*> m_services;
	QString authToken;
	int workspaceId;

	QQueue<RestResource::RequestRef> m_queuedRequests;

	QMutex requestMutex;
	QMutex nextMutex;
	QMutex cancelMutex;
};

#endif // REST_DISPATCHER_H
