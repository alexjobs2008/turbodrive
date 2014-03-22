#ifndef RESTRESOURCE_H
#define RESTRESOURCE_H

#include <QtCore/QObject>
#include <QtCore/QWeakPointer>
#include <QtCore/QByteArray>
#include <QtNetwork/QNetworkAccessManager>

#include "RestTypedefs.h"

class GeneralRestDispatcher;

class RestResource;
typedef QSharedPointer<RestResource> RestResourceRef;

class GenericRestResource;
typedef QSharedPointer<GenericRestResource> GenericRestResourceRef;

template <typename T1, typename T2> struct QPair;

class RestResource : public QObject
{
	Q_OBJECT
public:
	typedef QPair<QByteArray, QByteArray> HeaderPair;
	typedef QPair<QByteArray, QByteArray> ParamPair;
	typedef QList<ParamPair> ParamList;
	typedef QList<HeaderPair> HeaderList;
	typedef QNetworkAccessManager::Operation Operation;

	struct Request
	{
		Request(const RestResourceRef& restResource
			, Operation operation
			, const QString& service
			, const QString& path
			, const QByteArray& data
			, const HeaderList& headers);

		Request(const RestResourceRef& restResource
			, Operation operation
			, const QString& service
			, const QString& path
			, const ParamList& params
			, const HeaderList& headers);

		//~Request();

		RestResourceRef resource;
		Operation operation;
		QString service;
		QString path;
		QByteArray data;
		HeaderList headers;
		ParamList params;
		bool isCanceled;
	};

	typedef QSharedPointer<Request> RequestRef;

	struct Reply
	{
		Reply(const RequestRef& restResourceRequest
			, QNetworkReply* reply);

		Reply(const RestResourceRef& restResource
			, Operation restOperation
			, QNetworkReply* reply);

		//~Reply();

		RestResourceRef resource;
		Operation operation;
		QNetworkReply* reply;
	};

	typedef QSharedPointer<Reply> ReplyRef;

public:
	virtual ~RestResource();

public:
	void cancelAll() const;

public:
	static const QByteArray contentTypeHeader;
	static const QByteArray contentLengthHeader;
	static const QByteArray locationHeader;
	static const QByteArray lastModifiedHeader;
	static const QByteArray cookieHeader;
	static const QByteArray setCookieHeader;
	static const QByteArray acceptHeader;
	static const QByteArray authorizationHeader;
	static const QByteArray eTagHeader;
	static const QByteArray ifNoneMatchHeader;
	static const QByteArray userAgentHeader;
	static const QByteArray authTokenHeader;
	static const QByteArray workspaceHeader;
	static const QByteArray referer;

signals:
	void restOperationFailed(const RestResourceRef& restResource, const QString& errorMessage);
	void restOperationUnknownStatus(const RestResourceRef& restResource, int status);
	void restOperationCancelled(const RestResourceRef& restResource);

protected:
	explicit RestResource(QObject *parent = 0);

	template<class T>
	static QSharedPointer<T> create()
	{
		QSharedPointer<T> restResource(new T());
		restResource->_self = restResource.template staticCast<RestResource>();
		return restResource;
	}

	RestResourceRef self() const;

	// takes raw data as body
	RequestRef doOperation(int operation // RestResource::Operation
						, const QByteArray& data
						, const HeaderList& headers) const;

	// takes params name:value pairs
	RequestRef doOperation(int operation // RestResource::Operation
		, const ParamList& params
		, const HeaderList& headers) const;

	// takes params name:value pairs,
	// encodes as JSON and puts as paramContainerName value
	RestResource::RequestRef
		doOperation(int operation, const ParamList& params,
		const QString& paramContainerName, const HeaderList& headers) const;

	static QString getDataFromJson(const QByteArray& data);
	static QByteArray encodeParamsAsJson(const ParamList &params);

protected:
	QString base;

private:
	friend class GeneralRestDispatcher;
	void requestFinished(const ReplyRef& requestReply, bool& authenticationRequired);
	void requestCancelled();

	// To be implemented in derived classes
	virtual bool processGetResponse(int status, const QByteArray& data, const HeaderList& headers);
	virtual bool processPutResponse(int status, const QByteArray& data, const HeaderList& headers);
	virtual bool processPostResponse(int status, const QByteArray& data, const HeaderList& headers);
	virtual bool processDelResponse(int status, const QByteArray& data, const HeaderList& headers);
	virtual bool processHeadResponse(int status, const QByteArray& data, const HeaderList& headers);

	virtual QString path() const;
	virtual QString service() const = 0;
	virtual bool restricted() const = 0;

private:
	QWeakPointer<RestResource> _self;
};

template <class T>
void clearSharedResource(T& resource)
{
	if (!resource.isNull())
	{
		resource->cancelAll();
		resource.clear();
	}
}

#endif // RESTRESOURCE_H
