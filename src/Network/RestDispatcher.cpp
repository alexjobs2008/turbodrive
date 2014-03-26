#include "RestDispatcher.h"
#include "RestNetworkAccessManager.h"
#include "RestService.h"
#include "Settings/settings.h"
#include "QsLog/QsLog.h"

#include <QtCore/qglobal.h>
#include <QtCore/QThread>
#include <QtCore/QBuffer>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QCoreApplication>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslError>

using namespace Drive;

GeneralRestDispatcher& GeneralRestDispatcher::instance()
{
	static GeneralRestDispatcher restDispatcher;
	return restDispatcher;
}

GeneralRestDispatcher::GeneralRestDispatcher(QObject *parent)
	: QObject(parent)
	, mode(Mode::Unauthorized)
	, networkAccessManager(new RestNetworkAccessManager(this))
	, authToken(QString())
	//_authenticationHelper(new RestAuthenticationHelper(this)),
	//_mode(Unknown)
{
	connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(replyFinished(QNetworkReply*)));
	connect(networkAccessManager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
		this, SLOT(onSslErrors(QNetworkReply*,const QList<QSslError>&)));

	//connect(networkAccessManager, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&,QAuthenticator*)), this, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));

	//connect(_authenticationHelper, SIGNAL(reconnectSucceded()), this, SLOT(onDispatcherAuthorized()));
	//connect(_authenticationHelper, SIGNAL(reconnectFailed(RestDispatcher::ReconnectError)), this, SLOT(onDispatcherReconnectFailed(RestDispatcher::ReconnectError)));

	loadServices();
}

void GeneralRestDispatcher::loadServices()
{
	cancelAll();

	qDeleteAll(services);
	services.clear();

	int currentEnv = Settings::instance().get(Settings::env).toInt();

	QLOG_TRACE() << "Environment:" << currentEnv;

	switch(currentEnv)
	{
	case Settings::Env::AssistentDotThVersionX:
		services.insert("AuthService", new RestService("AuthService", "http://auth.assistent.th", this));
		services.insert("DashboardService", new RestService("DashboardService", "http://c0-0-40.assistent.th", this));
		services.insert("ProfileService", new RestService("ProfileService", "http://c0-0-40.api.assistent.th", this));
		services.insert("FilesService", new RestService("FilesService", "http://c0-0-40.files.assistent.th", this));
		services.insert("ContentService", new RestService("ContentService", "http://c0-0-40.files.assistent.th", this));
		services.insert("SharingService", new RestService("SharingService", "http://c0-0-40.files.assistent.th", this));
		services.insert("NotificationService", new RestService("NotificationService", "http://rpl.c0-0-40.files.assistent.th", this));
		break;
	case Settings::Env::AssistentDotTh:
		services.insert("AuthService", new RestService("AuthService", "http://auth.assistent.th", this));
		services.insert("DashboardService", new RestService("DashboardService", "http://assistent.th", this));
		services.insert("ProfileService", new RestService("ProfileService", "http://api.assistent.th", this));
		services.insert("FilesService", new RestService("FilesService", "http://files.assistent.th", this));
		services.insert("ContentService", new RestService("ContentService", "http://files.assistent.th", this));
		services.insert("SharingService", new RestService("SharingService", "http://files.assistent.th", this));
		services.insert("NotificationService", new RestService("NotificationService", "http://rpl.files.assistent.th", this));
		break;
	case Settings::Env::AssistentDotBy:
		services.insert("AuthService", new RestService("AuthService", "https://auth.assistent.by", this));
		services.insert("DashboardService", new RestService("DashboardService", "https://dashboard.assistent.by", this));
		services.insert("ProfileService", new RestService("ProfileService", "https://api.assistent.by", this));
		services.insert("FilesService", new RestService("FilesService", "https://files.assistent.by", this));
		services.insert("ContentService", new RestService("ContentService", "https://files.assistent.by", this));
		services.insert("SharingService", new RestService("SharingService", "https://files.assistent.by", this));
		break;
	case Settings::Env::NewAssistentDotBy:
		services.insert("AuthService", new RestService("AuthService", "https://auth.new.assistent.by", this));
		services.insert("DashboardService", new RestService("DashboardService", "http://new.assistent.by", this));
		services.insert("ProfileService", new RestService("ProfileService", "http://api.new.assistent.by", this));
		services.insert("FilesService", new RestService("FilesService", "http://disk.new.assistent.by", this));
		services.insert("ContentService", new RestService("ContentService", "http://disk.new.assistent.by", this));
		services.insert("SharingService", new RestService("SharingService", "http://disk.new.assistent.by", this));
		services.insert("NotificationService", new RestService("NotificationService", "http://rpl.disk.new.assistent.by", this));

//		services.insert("AuthService", new RestService("AuthService", "http://auth.assistent.yutas", this));
//		services.insert("DashboardService", new RestService("DashboardService", "http://assistent.yutas", this));
//		services.insert("ProfileService", new RestService("ProfileService", "http://api.assistent.yutas", this));
//		services.insert("FilesService", new RestService("FilesService", "http://files.assistent.yutas", this));
//		services.insert("ContentService", new RestService("ContentService", "http://files.assistent.yutas", this));
//		services.insert("SharingService", new RestService("SharingService", "http://files.assistent.yutas", this));
//		services.insert("NotificationService", new RestService("NotificationService", "http://rpl.files.assistent.yutas", this));
//
		break;
	case Settings::Env::TurbocloudDotRu:
		services.insert("AuthService", new RestService("AuthService", "https://auth.turbocloud.ru", this));
		services.insert("DashboardService", new RestService("DashboardService", "https://dashboard.turbocloud.ru", this));
		services.insert("ProfileService", new RestService("ProfileService", "https://api.turbocloud.ru", this));
		services.insert("FilesService", new RestService("FilesService", "https://files.turbocloud.ru", this));
		services.insert("ContentService", new RestService("ContentService", "https://files.turbocloud.ru", this));
		services.insert("SharingService", new RestService("SharingService", "https://files.turbocloud.ru", this));
		break;
	default:
		Q_ASSERT(false);
		break;
	}

	// _services.insert("", new RestService(this, "", ""));

	authTimestamp = QDateTime();
}

void GeneralRestDispatcher::cancelAll()
{
	QList<RestService*> servicesList = services.values();
	foreach(RestService* service, servicesList)
	{
		cancelCurrent(service);
		cancelAll(service->authenticatedRequests);
		cancelAll(service->unauthenticatedRequests);
	}

	networkAccessManager->abortAllRequests();
}

void GeneralRestDispatcher::cancelAll(const RestResourceRef& restResource)
{
	QMutexLocker locker(&cancelMutex);

	QList<RestService*> servicesList = services.values();
	foreach(RestService* service, servicesList)
	{
		cancelCurrent(service, &restResource);
		cancelAll(service->authenticatedRequests, &restResource);
		cancelAll(service->unauthenticatedRequests, &restResource);
	}
}

void GeneralRestDispatcher::cancelCurrent(RestService *service,
								const RestResourceRef *restResource)
{
	if (!service->currentRequest.isNull())
		if (!restResource || service->currentRequest->resource == *restResource)
		{
			QLOG_INFO() << "Current resource request canceled";

			if (restResource)
				restResource->data()->requestCancelled();
			else
				service->currentRequest->resource->requestCancelled();

			service->currentRequest->isCanceled = true;
		}
}

void GeneralRestDispatcher::cancelAll(QQueue<RestResource::RequestRef>& queue,
							const RestResourceRef* restResource)
{
	if (!restResource)
	{
		queue.clear();
	}
	else
	{
		for(QQueue<RestResource::RequestRef>::iterator iter = queue.begin();
			iter != queue.end();)
		{
			RestResource::RequestRef resourceRequest = *iter;
			if (resourceRequest->resource == *restResource)
			{
				iter = queue.erase(iter);
				resourceRequest.clear();
				QLOG_INFO() << "Resource request canceled";
			}
			else
			{
				++iter;
			}
		}
	}
}

void GeneralRestDispatcher::request(const RestResource::RequestRef &restRequest)
{
	QMutexLocker locker(&requestMutex);

	QLOG_TRACE() << "RestDispatcher::request:"
		<< restRequest->path << ". Params: " << restRequest->params;

	Q_ASSERT(services.contains(restRequest->service));
	RestService* service = services.value(restRequest->service);

	switch(mode)
	{
	case Authorized:
		//QLOG_TRACE() << "RestDispatcher::request(): adding request to authenticatedRequests queue for service" << service->name();
		service->authenticatedRequests.enqueue(restRequest);
		//QLOG_TRACE() << "Added. Queue size: " << service->authenticatedRequests.size();
		break;
	case Unauthorized:
		if (restRequest->resource->restricted())
		{
			//QLOG_TRACE() << "RestDispatcher::request(): adding request to authenticatedRequests queue for service" << service->name();
			service->authenticatedRequests.enqueue(restRequest);
			//QLOG_TRACE() << "Added. Queue size: " << service->authenticatedRequests.size();
		}
		else
		{
			//QLOG_TRACE() << "RestDispatcher::request(): adding request to unauthenticatedRequests queue for service" << service->name();
			service->unauthenticatedRequests.enqueue(restRequest);
			//QLOG_TRACE() << "Added. Queue size: " << service->unauthenticatedRequests.size();
		}
		break;
	default:
		Q_ASSERT(false);
		break;
	}

	//QLOG_TRACE() << "RestDispatcher::request() calling next(). Current thread:" << QThread::currentThreadId();

	next();
}

void GeneralRestDispatcher::next()
{
	// uncomment QLOG_TRACE() and requestCount lines for advanced logging

	//QLOG_TRACE() << "RestDispatcher::next(). Current thread:" << QThread::currentThreadId();

	QMutexLocker locker(&nextMutex);

	//QLOG_TRACE() << "RestDispatcher::next(). Passed mutex lock. Current thread:" << QThread::currentThreadId();

//	int requestCount = 0;

	QList<RestService*> serviceList = services.values();
	foreach(RestService* service, serviceList)
	{
		if (!service->currentRequest.isNull())
		{
			QLOG_TRACE() << "current request:" << service->currentRequest->service << service->currentRequest->resource->path();
		}

		while (service->currentRequest.isNull()) /// !!!
		{
		/* If we're Authenticated, then consume the Unauthenticated queue first
		and then the Authenticated queue. If we're Unauthenticated,
		then consume the Unauthenticated queue only. */
			if (mode == Authorized)
			{
				if (service->queuesAreEmpty())
				{
					//QLOG_TRACE() << "Both queues ARE empty for" << service->name();
					break;
				}

				if (!service->unauthenticatedRequests.isEmpty())
				{
					QLOG_TRACE() << "unauthenticatedRequests queue isn't empty for" << service->name();

					service->currentRequest =
						service->unauthenticatedRequests.dequeue();
				}
				else
				{
					QLOG_TRACE() << "authenticatedRequests queue isn't empty for" << service->name();

					service->currentRequest =
						service->authenticatedRequests.dequeue();
				}

				QLOG_TRACE() << "current request:" << service->currentRequest->service << service->currentRequest->resource->path();
			}
			else // mode == Unauthorized
			{
				if (service->unauthenticatedRequests.isEmpty())
					break;

				service->currentRequest =
					service->unauthenticatedRequests.dequeue();
			}

//			QLOG_TRACE() << "RestDispatcher::next() doing operation"
//				<< service->currentRequest->operation;

			doOperation(service->currentRequest->operation, service);

//			requestCount++;
		}
	}

//	QLOG_TRACE() << "RestDispatcher::next() requests sent: " << requestCount;
}

void GeneralRestDispatcher::setMode(Mode newMode)
{
	if (mode == newMode)
		return;

	QLOG_TRACE() << "RestDispatcher: setting mode: " << newMode;

	mode = newMode;

	switch(mode)
	{
	case Unauthorized:
		authTimestamp = QDateTime();
		emit dispatcherUnauthorized();
		break;
	case Authorized:
		emit dispatcherAboutToBeAuthorized();
		authTimestamp = QDateTime::currentDateTime().toUTC();
		emit dispatcherAuthorized();
		break;
	default:
		Q_ASSERT(false);
		break;
	}
}


QUrl GeneralRestDispatcher::buildUrl(const QString& serviceName,
									const QString &path,
									const RestResource::ParamList &params)
									const
{
	if(serviceName.isEmpty())
	{
		//maybe a generic resource
		return QUrl(path);
	}

	QString strBaseUrl = services.contains(serviceName) ?
		services.value(serviceName)->address() : "";

	Q_ASSERT(!strBaseUrl.isEmpty());

	QUrl url(strBaseUrl);
	QString basePath = url.path();

	// removing "/"s in the end and in the beginning
	int pos = basePath.length();
	while(pos && basePath.at(pos - 1) == '/') --pos;
	basePath.remove(pos, basePath.length() - pos);
	pos = 0;
	while(pos < path.length() && path.at(pos) == '/') ++pos;

	url.setPath(basePath + "/" + QString(path).remove(0, pos), QUrl::StrictMode);

	if (!params.isEmpty())
	{
		QUrlQuery urlQuery;
		foreach (RestResource::ParamPair param, params)
		{
			urlQuery.addQueryItem(param.first, param.second);
		}
		url.setQuery(urlQuery);
	}

	// QLOG_TRACE() << "Built URL: " << url.toString();

	return url;
}

QUrl GeneralRestDispatcher::buildUrl(const RestResource* restResource)
{
	return buildUrl(restResource->service(), restResource->path());
}

void GeneralRestDispatcher::setAuthToken(const QString& token)
{
	authToken = token;
	setMode(Authorized);
}

void GeneralRestDispatcher::setWorkspaceId(int workspaceId)
{
	this->workspaceId = workspaceId;
}

QList<QNetworkCookie> GeneralRestDispatcher::getCookies()
{
	return networkAccessManager->cookieJar()->cookiesForUrl(QUrl(COOKIE_URL));
}

QNetworkRequest GeneralRestDispatcher::createRequest(RestService* service) const
{
	QUrl url;
	QNetworkRequest request;

	if (service->currentRequest->operation
			== QNetworkAccessManager::PostOperation
		|| service->currentRequest->operation
			== QNetworkAccessManager::PutOperation
		|| service->currentRequest->operation
			== QNetworkAccessManager::DeleteOperation)
	{
		// no need to handle query params for POST, PUT and DELETE
		// as they already handled
		request = QNetworkRequest(buildUrl(service->currentRequest->service,
			service->currentRequest->path));
	}
	else
	{
		request = QNetworkRequest(buildUrl(service->currentRequest->service,
			service->currentRequest->path, service->currentRequest->params));
	}

	foreach (RestResource::HeaderPair header, service->currentRequest->headers)
	{
		request.setRawHeader(header.first, header.second);
	}

	if (!authToken.isEmpty())
	{
		request.setRawHeader(RestResource::authTokenHeader, authToken.toUtf8());

		request.setRawHeader(RestResource::workspaceHeader,
			QString("%1").arg(workspaceId).toUtf8());
	}

	request.setRawHeader(RestResource::userAgentHeader,
		QString("Desktop client %1").arg(QCoreApplication::applicationVersion()).toUtf8());

	request.setOriginatingObject(service);

	return request;
}

QUrlQuery GeneralRestDispatcher::createParams(RestResource::ParamList &params)
{
	if (params.isEmpty())
		return QUrlQuery();

	QUrlQuery urlQuery;

	foreach (RestResource::ParamPair param, params)
	{
		urlQuery.addQueryItem(param.first, param.second);
	}

	return urlQuery;
}

void GeneralRestDispatcher::doOperation(RestResource::Operation operation,
								RestService* service)
{
	QLOG_TRACE() << "RestDispatcher:doOperation" << service->currentRequest->path;

	if (operation == QNetworkAccessManager::PostOperation
		|| operation == QNetworkAccessManager::PutOperation
		|| operation == QNetworkAccessManager::DeleteOperation)
	{
		QByteArray bodyData;

		if (!service->currentRequest->params.isEmpty())
		{
			QUrlQuery urlQuery =
				createParams(service->currentRequest->params);

			if (!urlQuery.isEmpty())
			{
				bodyData =
					urlQuery.toString(QUrl::FullyEncoded).toUtf8();

				QLOG_TRACE() << "bodyData: " << bodyData;
			}
			else
			{
				QLOG_ERROR() << "Failed to convert request params.";
				service->currentRequest.clear();
				return;
		}
		}
		else
		{
			bodyData = service->currentRequest->data;
		}

		if (operation == QNetworkAccessManager::PostOperation)
		{
			networkAccessManager->post(createRequest(service), bodyData);
			QLOG_TRACE() << "RestDispatcher:doOperation: POST request sent";
		}
		else if (operation == QNetworkAccessManager::PutOperation)
		{
			networkAccessManager->put(createRequest(service), bodyData);
			QLOG_TRACE() << "RestDispatcher:doOperation: PUT request sent";
		}
		else if (operation == QNetworkAccessManager::DeleteOperation)
		{
			QBuffer *buffer =
				new QBuffer(service->currentRequest.data()->resource.data());

			buffer->setData(bodyData);

			//QLOG_TRACE() << "HTTP DELETE" << bodyData;

			QNetworkRequest request = createRequest(service);

			networkAccessManager->sendCustomRequest(request
				, QString("DELETE").toLatin1()
				, buffer);

			QLOG_TRACE() << "RestDispatcher:doOperation: DELETE request sent";
		}

	}
	else if (operation == QNetworkAccessManager::GetOperation)
	{
		networkAccessManager->get(createRequest(service));
		QLOG_TRACE() << "RestDispatcher:doOperation: GET request sent";
	}
	else if (operation == QNetworkAccessManager::DeleteOperation)
	{
		networkAccessManager->deleteResource(createRequest(service));
		QLOG_TRACE() << "RestDispatcher:doOperation: DELETE request sent";
	}
	else if (operation == QNetworkAccessManager::HeadOperation)
	{
		networkAccessManager->head(createRequest(service));
		QLOG_TRACE() << "RestDispatcher:doOperation: HEAD request sent";
	}
	else if (operation == QNetworkAccessManager::CustomOperation)
	{
		QLOG_TRACE() << "RestDispatcher: custom operation, nothing to do.";
		service->currentRequest.clear();
	}
}

void GeneralRestDispatcher::replyFinished(QNetworkReply* networkReply)
{
	QLOG_TRACE() << "replyFinished: " << networkReply
		<< networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()
		<< networkReply->url().toString();

	if (networkReply->error() != QNetworkReply::NoError)
	{
		QLOG_TRACE() << "Error: " << networkReply->error();
	}

	QObject* originatingObject = networkReply->request().originatingObject();

	if(!originatingObject)
	{
		// this should never happen
		QLOG_TRACE() << "Reply discarded because no originating object found.";
		return;
	}

	RestService* service = qobject_cast<RestService*>(originatingObject);
	if(!service)
	{
		// this should never happen also
		QLOG_TRACE() <<
			"Reply discarded because originating object isn't a service.";
		return;
	}


	if (service->currentRequest.isNull())
	{
		QLOG_TRACE() <<
			"Reply discarded because current service request is null.";

		next();
		return;
	}

	if (!service->currentRequest->isCanceled)
	{
		bool authenticationRequired = false;

		service->currentRequest->resource->requestFinished(
			RestResource::ReplyRef(
				new RestResource::Reply(service->currentRequest, networkReply))
			, authenticationRequired);

		if (authenticationRequired)
		{
			QLOG_TRACE () << "Redirecting...";

			service->authenticatedRequests.insert(
				service->authenticatedRequests.begin()
				, service->currentRequest);

			service->currentRequest.clear();

			setMode(Unauthorized);
			return;
		}
	}
	else
	{
		QLOG_INFO() << "Resource request is already canceled.";
	}

	service->currentRequest.clear();

	QLOG_TRACE() << "RestDispatcher::replyFinished() " << networkReply << ", calling next()";
	next();
}

void GeneralRestDispatcher::onSslErrors(QNetworkReply *reply, const QList<QSslError> & errors)
{
	for (int i = 0; i < errors.size(); i++)
	{
		QSslError error = errors.at(i);
		QLOG_INFO() << "SSL Error: " << error.errorString();
	}

	reply->ignoreSslErrors();
}