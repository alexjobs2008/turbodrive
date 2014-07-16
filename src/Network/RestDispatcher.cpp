#include "RestDispatcher.h"
#include "AppController.h"
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
	, m_watchDog([this] { QLOG_ERROR() << "Connection has been lost."; cancelAll(); })
{
	qRegisterMetaType<RestResource::RequestRef>("RequestRef");

	connect(networkAccessManager, &QNetworkAccessManager::finished,
			this, &GeneralRestDispatcher::replyFinished);
	connect(networkAccessManager, &QNetworkAccessManager::sslErrors,
			this, &GeneralRestDispatcher::onSslErrors);
}

void GeneralRestDispatcher::onServices(const QHash<QString, RestService*>& services)
{
	m_services = services;
	for (const auto& r: m_queuedRequests)
	{
		request(r);
	}
	m_queuedRequests.clear();
}

void GeneralRestDispatcher::cancelAll()
{
	QList<RestService*> servicesList = m_services.values();
	foreach(RestService* service, servicesList)
	{
		cancelCurrent(service);
		cancelAll(service->m_authenticatedRequests);
		cancelAll(service->m_unauthenticatedRequests);
	}

	networkAccessManager->abortAllRequests();
}

void GeneralRestDispatcher::cancelAll(const RestResourceRef& restResource)
{
	QMutexLocker locker(&cancelMutex);

	QList<RestService*> servicesList = m_services.values();
	foreach(RestService* service, servicesList)
	{
		cancelCurrent(service, &restResource);
		cancelAll(service->m_authenticatedRequests, &restResource);
		cancelAll(service->m_unauthenticatedRequests, &restResource);
	}
}

void GeneralRestDispatcher::cancelCurrent(RestService *service,
								const RestResourceRef *restResource)
{
	if (!service->m_currentRequest.isNull())
		if (!restResource || service->m_currentRequest->resource == *restResource)
		{
			if (restResource)
				restResource->data()->requestCancelled();
			else
				service->m_currentRequest->resource->requestCancelled();

			service->m_currentRequest->isCanceled = true;
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

	if (m_services.isEmpty())
	{
		m_queuedRequests.push_back(restRequest);
		return;
	}

	Q_ASSERT(m_services.contains(restRequest->service));
	RestService* service = m_services.value(restRequest->service);

	switch(mode)
	{
	case Authorized:
		service->m_authenticatedRequests.enqueue(restRequest);
		break;
	case Unauthorized:
		if (restRequest->resource->restricted())
		{
			service->m_authenticatedRequests.enqueue(restRequest);
		}
		else
		{
			service->m_unauthenticatedRequests.enqueue(restRequest);
		}
		break;
	default:
		Q_ASSERT(false);
		break;
	}

	next();
}

void GeneralRestDispatcher::next()
{
	QMutexLocker locker(&nextMutex);

	QList<RestService*> serviceList = m_services.values();
	foreach(RestService* service, serviceList)
	{
		while (service->m_currentRequest.isNull()) /// !!!
		{
		/* If we're Authenticated, then consume the Unauthenticated queue first
		and then the Authenticated queue. If we're Unauthenticated,
		then consume the Unauthenticated queue only. */
			if (mode == Authorized)
			{
				if (service->queuesAreEmpty())
				{
					break;
				}

				if (!service->m_unauthenticatedRequests.isEmpty())
				{
					service->m_currentRequest =
						service->m_unauthenticatedRequests.dequeue();
				}
				else
				{
					service->m_currentRequest =
						service->m_authenticatedRequests.dequeue();
				}

			}
			else // mode == Unauthorized
			{
				if (service->m_unauthenticatedRequests.isEmpty())
					break;

				service->m_currentRequest =
					service->m_unauthenticatedRequests.dequeue();
			}

			doOperation(service->m_currentRequest->operation, service);
		}
	}
}

void GeneralRestDispatcher::setMode(Mode newMode)
{
	if (mode == newMode)
		return;

	mode = newMode;

	switch(mode)
	{
	case Unauthorized:
		Q_EMIT dispatcherUnauthorized();
		break;
	case Authorized:
		Q_EMIT dispatcherAuthorized();
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

	QString strBaseUrl = m_services.contains(serviceName) ?
		m_services.value(serviceName)->address() : "";

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

	if (service->m_currentRequest->operation
			== QNetworkAccessManager::PostOperation
		|| service->m_currentRequest->operation
			== QNetworkAccessManager::PutOperation
		|| service->m_currentRequest->operation
			== QNetworkAccessManager::DeleteOperation)
	{
		// no need to handle query params for POST, PUT and DELETE
		// as they already handled
		request = QNetworkRequest(buildUrl(service->m_currentRequest->service,
			service->m_currentRequest->path));
	}
	else
	{
		request = QNetworkRequest(buildUrl(service->m_currentRequest->service,
			service->m_currentRequest->path, service->m_currentRequest->params));
	}

	foreach (RestResource::HeaderPair header, service->m_currentRequest->headers)
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
	if (operation == QNetworkAccessManager::PostOperation
		|| operation == QNetworkAccessManager::PutOperation
		|| operation == QNetworkAccessManager::DeleteOperation)
	{
		QByteArray bodyData;

		if (!service->m_currentRequest->params.isEmpty())
		{
			QUrlQuery urlQuery =
				createParams(service->m_currentRequest->params);

			if (!urlQuery.isEmpty())
			{
				bodyData =
					urlQuery.toString(QUrl::FullyEncoded).toUtf8();
			}
			else
			{
				QLOG_ERROR() << "Failed to convert request params.";
				service->m_currentRequest.clear();
				return;
		}
		}
		else
		{
			bodyData = service->m_currentRequest->data;
		}
		QLOG_INFO() << "Service: " << service->toString();
		QLOG_INFO() << "Request body: " << bodyData;

		if (operation == QNetworkAccessManager::PostOperation)
		{
			networkAccessManager->post(createRequest(service), bodyData);
			m_watchDog.restart();
		}
		else if (operation == QNetworkAccessManager::PutOperation)
		{
			networkAccessManager->put(createRequest(service), bodyData);
			m_watchDog.restart();
		}
		else if (operation == QNetworkAccessManager::DeleteOperation)
		{
			QBuffer *buffer =
				new QBuffer(service->m_currentRequest.data()->resource.data());

			buffer->setData(bodyData);

			QNetworkRequest request = createRequest(service);

			networkAccessManager->sendCustomRequest(request
				, QString("DELETE").toLatin1()
				, buffer);
			m_watchDog.restart();
		}

	}
	else if (operation == QNetworkAccessManager::GetOperation)
	{
		networkAccessManager->get(createRequest(service));
		m_watchDog.restart();
	}
	else if (operation == QNetworkAccessManager::DeleteOperation)
	{
		networkAccessManager->deleteResource(createRequest(service));
		m_watchDog.restart();
	}
	else if (operation == QNetworkAccessManager::HeadOperation)
	{
		networkAccessManager->head(createRequest(service));
		m_watchDog.restart();
	}
	else if (operation == QNetworkAccessManager::CustomOperation)
	{
		service->m_currentRequest.clear();
	}
}

void GeneralRestDispatcher::replyFinished(QNetworkReply* networkReply)
{
	m_watchDog.stop();

	QObject* originatingObject = networkReply->request().originatingObject();

	if(!originatingObject)
	{
		// this should never happen
		QLOG_ERROR() << "Reply discarded because no originating object found.";
		return;
	}

	RestService* service = qobject_cast<RestService*>(originatingObject);
	if(!service)
	{
		// this should never happen also
		QLOG_ERROR() <<
			"Reply discarded because originating object isn't a service.";
		return;
	}


	if (service->m_currentRequest.isNull())
	{
		QLOG_ERROR() <<
			"Reply discarded because current service request is null.";

		next();
		return;
	}

	if (!service->m_currentRequest->isCanceled)
	{
		bool authenticationRequired = false;

		service->m_currentRequest->resource->requestFinished(
			RestResource::ReplyRef(
				new RestResource::Reply(service->m_currentRequest, networkReply))
			, authenticationRequired);

		if (authenticationRequired)
		{
			service->m_authenticatedRequests.insert(
				service->m_authenticatedRequests.begin()
				, service->m_currentRequest);

			service->m_currentRequest.clear();

			setMode(Unauthorized);
			return;
		}
	}
	else
	{
		QLOG_INFO() << "Resource request is already canceled.";
	}

	service->m_currentRequest.clear();

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
