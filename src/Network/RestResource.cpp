#include "RestResource.h"
#include "RestDispatcher.h"

#include "QsLog/QsLog.h"

#include <QtNetwork/QNetworkReply>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

const QByteArray RestResource::contentTypeHeader   ("Content-Type");
const QByteArray RestResource::contentLengthHeader ("Content-Length");
const QByteArray RestResource::locationHeader	("Location");
const QByteArray RestResource::lastModifiedHeader  ("Last-Modified");
const QByteArray RestResource::cookieHeader		("Cookie");
const QByteArray RestResource::setCookieHeader	("Set-Cookie");
const QByteArray RestResource::acceptHeader		("Accept");
const QByteArray RestResource::authorizationHeader ("Authorization");
const QByteArray RestResource::eTagHeader		("ETag");
const QByteArray RestResource::ifNoneMatchHeader   ("If-None-Match");
const QByteArray RestResource::userAgentHeader	("User-Agent");
const QByteArray RestResource::authTokenHeader	("X-Sso-Token");
const QByteArray RestResource::workspaceHeader	("X-Current-Workspace-Id");
const QByteArray RestResource::referer			("Referer");

RestResource::Request::Request(const RestResourceRef& restResource
							, Operation operation
							, const QString& service
							, const QString& path
							, const QByteArray& data
							, const HeaderList& headers)
	: resource(restResource)
	, operation(operation)
	, service(service)
	, path(path)
	, data(data)
	, headers(headers)
	, isCanceled(false)
{
}

RestResource::Request::Request(const RestResourceRef& restResource
							, Operation operation
							, const QString& service
							, const QString& path
							, const ParamList& params
							, const HeaderList& headers)
	: resource(restResource)
	, operation(operation)
	, service(service)
	, path(path)
	, params(params)
	, headers(headers)
	, isCanceled(false)
{
}

QString RestResource::Request::toString() const
{
	QVariantMap map;

#define MAKE_PAIR(value) QLatin1String(#value), QVariant(value)
	map.insert(MAKE_PAIR(resource));
	map.insert(MAKE_PAIR(operation));
	map.insert(MAKE_PAIR(service));
	map.insert(MAKE_PAIR(path));
//	map.insert(MAKE_PAIR(params));
//	map.insert(MAKE_PAIR(headers));
	map.insert(MAKE_PAIR(isCanceled));
#undef MAKE_PAIR

	return QJsonDocument(QJsonObject::fromVariantMap(map)).toJson();
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
	QLOG_TRACE() << "resource created" << this;
}

RestResource::~RestResource()
{
	QLOG_TRACE() << "resource destroyed" << this;
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
		QLOG_ERROR() << requestReply->reply->request().url().toString()
			<< status << requestReply->reply->errorString();
	}

	if (status == 301)
	{
		QVariant locationHeader =
			requestReply->reply->header(QNetworkRequest::LocationHeader);

		if (locationHeader.isValid())
		{
			// TODO: redirect
		}
		else
		{
			QLOG_ERROR() << "301 Redirection has no Location header";
		}


	}

// 	if (status == 403 || status == 401)
//	{
//		QLOG_TRACE() << "Code" << status << ": authorization required";
//		authenticationRequired = true;
//		return;
//	}

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

RestResource::RequestRef
	RestResource::doOperation(int operation, const QByteArray& data,
		const HeaderList& headers) const
{
	RequestRef restRequest(new Request(self(), static_cast<Operation>(operation), service(), path(), data, headers));
	Q_EMIT request(restRequest);
	return restRequest;
}

RestResource::RequestRef
	RestResource::doOperation(int operation, const ParamList& params,
		const HeaderList& headers) const
{
	RequestRef restRequest(new Request(self(), static_cast<Operation>(operation), service(), path(), params, headers));
	Q_EMIT request(restRequest);
	return restRequest;
}

RestResource::RequestRef
	RestResource::doOperation(int operation, const ParamList& params,
		const QString& paramContainerName, const HeaderList& headers) const
{
	QByteArray json = encodeParamsAsJson(params);
	ParamList actualParam;
	actualParam.append(ParamPair(paramContainerName.toUtf8(), json));

	return doOperation(operation, actualParam, headers);
}

QString RestResource::getDataFromJson(const QByteArray& data)
{
	QString result;

	QJsonDocument doc = QJsonDocument::fromJson(data);
	if (doc.isObject())
	{
		QJsonObject jo = QJsonDocument::fromJson(data).object();

		if (jo.contains("data"))
		{
			QJsonValue value = jo.value("data");

			if (value.type() == QJsonValue::String)
				return value.toString();

			if (value.type() == QJsonValue::Array)
			{
				QJsonDocument doc(value.toArray());
				return doc.toJson();
			}

			if (value.type() == QJsonValue::Object)
			{
				QJsonDocument doc(value.toObject());
				return doc.toJson();
			}
		}
	}

	return result;
}

QByteArray RestResource::encodeParamsAsJson(const ParamList &params)
{
	QVariantMap map;
	foreach (ParamPair param, params)
	{
		map.insert(param.first, param.second);
	}

	QJsonObject jsonObject = QJsonObject::fromVariantMap(map);
	QJsonDocument doc(jsonObject);

	QLOG_TRACE() << "encoded params as JSON: " << doc.toJson(QJsonDocument::Compact);

	return doc.toJson(QJsonDocument::Compact);
}

void RestResource::init()
{
	GeneralRestDispatcher& dispatcher = GeneralRestDispatcher::instance();
	connect(this, &RestResource::request,
			&dispatcher, &GeneralRestDispatcher::request);
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
