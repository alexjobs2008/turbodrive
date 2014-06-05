#include "RestDispatcher.h"
#include "RestNetworkAccessManager.h"
#include "RestService.h"
#include "Settings/settings.h"
#include "QsLog/QsLog.h"

#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/qglobal.h>
#include <QtCore/QCoreApplication>

using namespace Drive;

// GeneralRestDispatcher& GeneralRestDispatcher::instance()
// {
//     static GeneralRestDispatcher restDispatcher;
//     return restDispatcher;
// }

GeneralRestDispatcher::GeneralRestDispatcher(QObject *parent)
    : QObject(parent)
    , mode(Mode::Unauthorized)
    , networkAccessManager(new RestNetworkAccessManager(this))
    , authToken(QString())
    //_authenticationHelper(new RestAuthenticationHelper(this)),
    //_mode(Unknown)
{
    connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    connect(networkAccessManager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), SLOT(onSslErrors(QNetworkReply*,const QList<QSslError>&)));
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
    case Settings::Env::Dev: 
        services.insert("AuthService", new RestService("AuthService", "https://files.assistent.th", this));
        break;
    case Settings::Env::Production:
        services.insert("AuthService", new RestService("AuthService", "https://master.files.assistent.th", this));
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


void GeneralRestDispatcher::request(const RestResource::RequestRef &restResourceRequest)
{
    QLOG_TRACE() << restResourceRequest->path;

    Q_ASSERT(services.contains(restResourceRequest->service));
    RestService* service = services.value(restResourceRequest->service);
    
    switch(mode)
    {
    case Authorized:
        service->authenticatedRequests.enqueue(restResourceRequest);
        break;
    case Unauthorized:
        if (restResourceRequest->resource->restricted())
            service->authenticatedRequests.enqueue(restResourceRequest);
        else
            service->unauthenticatedRequests.enqueue(restResourceRequest);
        break;
    default:
        Q_ASSERT(false);
        break;
    }

    next();
}

void GeneralRestDispatcher::next()
{
    QList<RestService*> serviceList = services.values();
    foreach(RestService* service, serviceList)
    {
        while (service->currentRequest.isNull())
        {
        /* If we're Authenticated, then consume the Unauthenticated queue first
           and then the Authenticated queue. If we're Unauthenticated,
           then consume the Unauthenticated queue only. */
            if (mode == Authorized)
            {
                if (service->queuesAreEmpty())
                    break;

                if (!service->unauthenticatedRequests.isEmpty())
                {
                    service->currentRequest =
                        service->unauthenticatedRequests.dequeue();
                }
                else
                {
                    service->currentRequest =
                        service->authenticatedRequests.dequeue();
                }
            }
            else // mode == Unauthorized
            {
                if (service->unauthenticatedRequests.isEmpty())
                    break;

                service->currentRequest =
                    service->unauthenticatedRequests.dequeue();
            }

            QLOG_TRACE() << "RestDispatcher::next() processing";
            doOperation(service->currentRequest->operation, service);
        }
    }
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


QUrl GeneralRestDispatcher::buildUrl(const QString& serviceName, const QString &path) const
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

    url.setPath(basePath + "/" + QString(path).remove(0, pos));
    return url;
}

QUrl GeneralRestDispatcher::buildUrl(const RestResource* restResource)
{
    return buildUrl(restResource->service(), restResource->path());
}

QNetworkRequest GeneralRestDispatcher::createRequest(RestService* service) const
{
    QNetworkRequest request(buildUrl(service->currentRequest->service,
        service->currentRequest->path));
    
    foreach (RestResource::HeaderPair header, service->currentRequest->headers)
    {
        request.setRawHeader(header.first, header.second);
    }
    if (!authToken.isEmpty())
    {
        request.setRawHeader(RestResource::cookieHeader,
            QString("PHPSESSID={%1}; path=/; domain=files.assistent.dev")
            .arg(authToken).toUtf8());
        
    }
    request.setRawHeader(RestResource::userAgentHeader,
        QString("Desktop client %1").arg(QCoreApplication::applicationVersion()).toUtf8());

    request.setOriginatingObject(service);

    QLOG_TRACE() << "Created request " << request.url();
    return request;

}

void GeneralRestDispatcher::doOperation(RestResource::Operation operation,
                                 RestService* service)
{
    switch(operation)
    {
    case QNetworkAccessManager::PostOperation:
        networkAccessManager->
            post(createRequest(service), service->currentRequest->data);
        break;
    case QNetworkAccessManager::GetOperation:
        networkAccessManager->get(createRequest(service));
        break;
    case QNetworkAccessManager::PutOperation:
        networkAccessManager->
            put(createRequest(service), service->currentRequest->data);
        break;
    case QNetworkAccessManager::DeleteOperation:
        networkAccessManager->deleteResource(createRequest(service));
        break;
    case QNetworkAccessManager::HeadOperation:
        networkAccessManager->head(createRequest(service));
        break;
    case QNetworkAccessManager::CustomOperation:
    default:
        QLOG_TRACE() << "RestDispatcher: custom operation, nothing to do.";
        service->currentRequest.clear();
        break;
    }

}

void GeneralRestDispatcher::replyFinished(QNetworkReply* networkReply)
{
    QLOG_TRACE() << "replyFinished. URL: " << networkReply->url().toString();
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
    next();
}
