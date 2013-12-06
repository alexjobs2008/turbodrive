#include "RestResource.h"
#include "RestDispatcher.h"

#include "QsLog/QsLog.h"

#include <QtNetwork/QNetworkReply>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

const QByteArray RestResource::contentTypeHeader   ("Content-Type");
const QByteArray RestResource::contentLengthHeader ("Content-Length");
const QByteArray RestResource::locationHeader      ("Location");
const QByteArray RestResource::lastModifiedHeader  ("Last-Modified");
const QByteArray RestResource::cookieHeader        ("Cookie");
const QByteArray RestResource::setCookieHeader     ("Set-Cookie");
const QByteArray RestResource::acceptHeader        ("Accept");
const QByteArray RestResource::authorizationHeader ("Authorization");
const QByteArray RestResource::eTagHeader          ("ETag");
const QByteArray RestResource::ifNoneMatchHeader   ("If-None-Match");
const QByteArray RestResource::userAgentHeader     ("User-Agent");
const QByteArray RestResource::authTokenHeader     ("X-Sso-Token");
const QByteArray RestResource::workspaceHeader     ("X-Current-Workspace-Id");
const QByteArray RestResource::referer             ("Referer");

RestResource::Request::Request(const RestResourceRef& restResource
                               , Operation operation
                               , const QString& service
                               , const QString& path
                               , const QByteArray& data
                               , const QList<QPair<QByteArray, QByteArray> >& headers)
        : resource(restResource)
        , operation(operation)
        , service(service)
        , path(path)
        , data(data)
        , headers(headers)
        , isCanceled(false)
{
}

RestResource::Reply::Reply(const RequestRef& restResourceRequest
                           , QNetworkReply* reply)
     : resource(restResourceRequest->resource)
     , operation(restResourceRequest->operation)
     , reply(reply)
{
}

RestResource::Reply::Reply(const RestResourceRef& restResource
                           , Operation restOperation
                           , QNetworkReply* reply)
    : resource(restResource)
    , operation(restOperation)
    , reply(reply)
{
}


RestResource::RestResource(QObject *parent)
	: QObject(parent)
{
}

RestResource::~RestResource()
{
}

RestResourceRef RestResource::self() const
{
    return _self.toStrongRef();
}

void RestResource::requestFinished(const ReplyRef& requestReply, bool& authenticationRequired)
{
    Q_ASSERT(requestReply->reply);

    if (!requestReply->reply)
    {
        return;
    }

    int status = requestReply->reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

	if (status == 0 || status >= 300) // log everything related to client/server errors and redirections
	{
		QLOG_TRACE() << requestReply->reply->request().url().toString()
			<< status << requestReply->reply->errorString();
	}
    
// 	if (status == 403 || status == 401)
//     {
//         QLOG_TRACE() << "Code" << status << ": authorization required";
//         authenticationRequired = true;
//         return;
//     }

    bool processed = false;

    QByteArray replyData = requestReply->reply->readAll();
    HeaderList headers = requestReply->reply->rawHeaderPairs();
    switch(requestReply->operation)
    {
    case QNetworkAccessManager::GetOperation:
        processed = processGetResponse(status, replyData, headers);
        break;
    case QNetworkAccessManager::PutOperation:
        processed = processPutResponse(status, replyData, headers);
        break;
    case QNetworkAccessManager::PostOperation:
        processed = processPostResponse(status, replyData, headers);
        break;
    case QNetworkAccessManager::DeleteOperation:
        processed = processDelResponse(status, replyData, headers);
        break;
    case QNetworkAccessManager::HeadOperation:
        processed = processHeadResponse(status, replyData, headers);
        break;
    case QNetworkAccessManager::CustomOperation:
    default:
        Q_ASSERT(false);
        break;
    }

    if (!processed)
    {
        if(requestReply->reply->error() != QNetworkReply::NoError)
        {
            //BACKEND_ERROR() <<requestReply->_reply->request().url().toString()<<  status<<  requestReply->_reply->errorString();
            emit restOperationFailed(self(), requestReply->reply->errorString());
        }
        else
        {
            //BACKEND_ERROR()<<requestReply->_reply->request().url().toString()<<  status;
            emit restOperationUnknownStatus(self(), status);
        }
    }
}

void RestResource::requestCancelled()
{
    QLOG_TRACE() << "requestCancelled() " << service() << " " << path();
    emit restOperationCancelled(self());
}

QString RestResource::path() const
{
    return QString();
}

RestResource::RequestRef RestResource::doOperation(int operation, const QByteArray& data, const HeaderList& headers) const
{
    RequestRef restRequest(new Request(self(), static_cast<Operation>(operation), service(), path(), data, headers));
    GeneralRestDispatcher::instance().request(restRequest);
    return restRequest;
}

QString RestResource::getDataFromJson(const QByteArray& data)
{
    QJsonObject jo = QJsonDocument::fromJson(data).object();
    QString result;

    if (jo.contains("data"))
        result = jo.value("data").toString();

    return result;
}

bool RestResource::processGetResponse(int status, const QByteArray& data, const HeaderList& headers)
{
    Q_UNUSED(data);
    Q_UNUSED(status);
    Q_UNUSED(headers);
    return false;
}

bool RestResource::processPutResponse(int status, const QByteArray& data, const HeaderList& headers)
{
    Q_UNUSED(data);
    Q_UNUSED(status);
    Q_UNUSED(headers);
    return false;
}

bool RestResource::processPostResponse(int status, const QByteArray& data, const HeaderList& headers)
{
    Q_UNUSED(data);
    Q_UNUSED(status);
    Q_UNUSED(headers);
    return false;
}

bool RestResource::processDelResponse(int status, const QByteArray& data, const HeaderList& headers)
{
    Q_UNUSED(data);
    Q_UNUSED(status);
    Q_UNUSED(headers);
    return false;
}

bool RestResource::processHeadResponse(int status, const QByteArray& data, const HeaderList& headers)
{
    Q_UNUSED(data);
    Q_UNUSED(status);
    Q_UNUSED(headers);
    return false;
}

void RestResource::cancelAll() const
{
    GeneralRestDispatcher::instance().cancelAll(self());
}