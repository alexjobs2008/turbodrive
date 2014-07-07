#include "FilesService.h"

#include "Settings/settings.h"
#include "APIClient/ApiTypes.h"
#include <Util/FileUtils.h>

#include "QsLog/QsLog.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include <QtCore/QUrlQuery>
#include <QtCore/QStringList>

#define FILES_SERVICE_NAME "FilesService"

namespace Drive
{

OnlineRestResourceRef OnlineRestResource::create()
{
	OnlineRestResourceRef resource =
		RestResource::create<OnlineRestResource>();
	return resource;
}

void OnlineRestResource::ping()
{
	QLOG_INFO() << "Going online...";

	doOperation(QNetworkAccessManager::GetOperation,
		QByteArray(), HeaderList());
}

QString OnlineRestResource::path() const
{
	return "_online";
}

QString OnlineRestResource::service() const
{
	return FILES_SERVICE_NAME;
}

bool OnlineRestResource::restricted() const
{
	return true;
}

bool OnlineRestResource::processGetResponse(int status,
										const QByteArray&,
										const HeaderList&)
{
	QLOG_DEBUG() << "Ping status " << status;

	if (status == 200)
	{
		emit pingOk();
	}
	else
	{
		emit pingError();
	}

	return true;
}

//--------------------------------------------------------------------------

GetAncestorsRestResourceRef GetAncestorsRestResource::create()
{
	GetAncestorsRestResourceRef resource =
		RestResource::create<GetAncestorsRestResource>();

	QLOG_TRACE() << "GetAncestorsRestResource created:" << resource.data();

	return resource;
}

void GetAncestorsRestResource::getAncestors(int fileObjectId)
{
	QLOG_TRACE() << "getAncestors requested for id:" << fileObjectId;

	fileId = fileObjectId;
	doOperation(QNetworkAccessManager::GetOperation,
		QByteArray(), HeaderList());
}

QString GetAncestorsRestResource::path() const
{
	return QString("/api/v1/files/getAncestors/%1").arg(fileId);
}

QString GetAncestorsRestResource::service() const
{
	return FILES_SERVICE_NAME;
}

bool GetAncestorsRestResource::restricted() const
{
	return true;
}

bool GetAncestorsRestResource::processGetResponse(int status,
												const QByteArray& data,
												const HeaderList&)
{
	QLOG_TRACE() << "getAncestors requested finished: " << status;

	if (status != 200)
	{
		emit failed();
		return true;
	}

	QString fullPath;
	QString arrayString = RestResource::getDataFromJson(data);
	QJsonDocument doc = QJsonDocument::fromJson(arrayString.toUtf8());

	if (doc.isArray())
	{
		QJsonArray array = doc.array();

		for (int i = 0; i < array.size(); i++)
		{
			QJsonValue value = array.at(i);
			if (value.type() == QJsonValue::Object)
			{
				QJsonObject obj = value.toObject();
				RemoteFileDesc fileDesc = RemoteFileDesc::fromJson(obj);
				if (!fileDesc.name.isEmpty())
				{
					if (i != 0)
					{
						fullPath.append(Utils::separator());
					}
					fullPath.append(fileDesc.name);
				}
			}
		}
	}

	emit succeeded(fullPath);

	return true;
}

//----------------------------------------------------------------------------

ContentRestResourceRef ContentRestResource::create()
{
	ContentRestResourceRef resource =
		RestResource::create<ContentRestResource>();
	return resource;
}

void ContentRestResource::download(int fileId)
{
	this->fileId = fileId;

	HeaderList headers;
	QByteArray data;

	doOperation(QNetworkAccessManager::GetOperation, data, headers);
}

QString ContentRestResource::path() const
{
	return QString("/api/v1/content/%1").arg(fileId);
}

QString ContentRestResource::service() const
{
	return FILES_SERVICE_NAME;
}

bool ContentRestResource::restricted() const
{
	return true;
}

bool ContentRestResource::processGetResponse(int status,
											const QByteArray& data,
											const HeaderList& headers)
{
	QLOG_TRACE() << "content get: " << status << "."
		<< headers << data;

	return true;
}

//----------------------------------------------------------------------------

CreateRestResourceRef CreateRestResource::create()
{
	CreateRestResourceRef resource =
		RestResource::create<CreateRestResource>();

	QLOG_TRACE() << "CreateRestResource created:" << resource.data();

	return resource;
}

void CreateRestResource::createFolder(int parentId, const QString& folderName)
{
	this->parentId = parentId;

	HeaderList headers;
	ParamList params;

	params.append(ParamPair("name", folderName.toUtf8()));
	params.append(ParamPair("parentId", QString::number(parentId).toUtf8()));
	params.append(ParamPair("type", "1"));

	doOperation(QNetworkAccessManager::PostOperation,
		params, "data", headers);
}

QString CreateRestResource::path() const
{
	return "/api/v1/files";
}

QString CreateRestResource::service() const
{
	return FILES_SERVICE_NAME;
}

bool CreateRestResource::restricted() const
{
	return true;
}

bool CreateRestResource::processPostResponse(int status,
											const QByteArray& data,
											const HeaderList&)
{
//		QLOG_TRACE() << "create post: " << status << "."
//			<< headers << data;

	if (status == 200)
	{
		QJsonDocument doc =
			QJsonDocument::fromJson(this->getDataFromJson(data).toUtf8());

		if (doc.isObject())
		{
			RemoteFileDesc fileDesc = RemoteFileDesc::fromJson(doc.object());
			if (fileDesc.isValid())
			{
				emit succeeded(fileDesc);
				return true;
			}
		}

		emit failed(QString::number(status).append(": ").append(data));
	}
	else
	{
		emit failed(QString::number(status).append(": ").append(data));
	}

	return true;
}

//----------------------------------------------------------------------------

GetChildrenResourceRef GetChildrenResource::create()
{
	return RestResource::create<GetChildrenResource>();
}


void GetChildrenResource::getChildren(int parentId)
{
	getChildren(QString::number(parentId));
}

void GetChildrenResource::getChildren(const QString& parentId)
{
	this->parentId = parentId;

	HeaderList headers;
	QByteArray data;

	doOperation(QNetworkAccessManager::GetOperation, data, headers);
}

void GetChildrenResource::getFileObjectId(const QString& remotePath)
{
	QLOG_TRACE() << "GetChildrenResource getFileObjectId for:" << remotePath;

	this->remotePath = remotePath;

	list = remotePath.split("/", QString::SkipEmptyParts);
	if (list.isEmpty())
	{
		QLOG_ERROR() << "Unable to get file object id for" << remotePath;
		emit getFileObjectIdFailed();
		return;
	}

	getFirstChildId();
}

void GetChildrenResource::getFileObjectParentId(const QString& remotePath)
{
	QLOG_TRACE() << "GetChildrenResource getFileObjectParentId for:" << remotePath;

	list = remotePath.split("/", QString::SkipEmptyParts);
	if (list.size() <= 2)
	{
		QLOG_ERROR() << "Unable to get file object parent id for"
			<< remotePath;

		emit getFileObjectIdFailed();
		return;
	}

	list.removeLast();
	getFileObjectId(list.join("/"));
}

QString GetChildrenResource::path() const
{
	return QString("/api/v1/files/getChildren/%1").arg(parentId);
}

QString GetChildrenResource::service() const
{
	return FILES_SERVICE_NAME;
}

bool GetChildrenResource::restricted() const
{
	return true;
}

bool GetChildrenResource::processGetResponse(int status,
											const QByteArray& data,
											const HeaderList&)
{
	if (status != 200)
	{
		emit failed();
		return true;
	}

	QString json = getDataFromJson(data);

	QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
	if (!doc.isArray())
	{
		emit failed();
		return true;
	}

	QList<RemoteFileDesc> list;

	QJsonArray array = doc.array();
	for (int i = 0; i < array.size(); i++)
	{
		QJsonValue value = array.at(i);
		if (value.type() == QJsonValue::Object)
		{
			QJsonObject obj = value.toObject();
			RemoteFileDesc fileDesc = RemoteFileDesc::fromJson(obj);
			if (fileDesc.isValid())
			{

				list << fileDesc;
			}
		}
	}

	emit succeeded(list);

	return true;
}

void GetChildrenResource::getFirstChildId()
{
	QLOG_TRACE() << "GetChildrenResource getFirstChildId";

	if (list.at(0) == "#root")
	{
		if (list.size() == 1)
		{
			emit getFileObjectIdSucceeded(0);
			return;
		}

		currentItem = 0;

		getNextChildId(0);
	}
	else
	{
		QLOG_ERROR() << "Unable to get file object id for" << remotePath
			<< "as it should start with #root";
		emit getFileObjectIdFailed();
	}
}

void GetChildrenResource::getNextChildId(int parentId)
{
	QLOG_TRACE() << "GetChildrenResource getNextChildId, parent id:" << parentId;

	currentItem++;

	if (currentItem >= list.size())
	{
		QLOG_ERROR() << "Failed to get file object id for" << remotePath;
		emit getFileObjectIdFailed();
	}

	GetChildIdResourceRef getChildIdResource = GetChildIdResource::create();

	connect(getChildIdResource.data(), &GetChildIdResource::succeeded,
		this, &GetChildrenResource::onGetChildIdSucceeded);

	connect(getChildIdResource.data(), &GetChildIdResource::failed,
		this, &GetChildrenResource::onGetChildIdFailed);

	//getNextChildId(getChildIdResource, 0);

	getChildIdResource->getChildId(parentId, list.at(currentItem));

	res = getChildIdResource;
}

void GetChildrenResource::onGetChildIdSucceeded(int id)
{
	if (currentItem == list.size() - 1)
	{
		emit getFileObjectIdSucceeded(id);
		return;
	}

	getNextChildId(id);
}

void GetChildrenResource::onGetChildIdFailed()
{
	QLOG_ERROR() << "Failed to get file object id for " << remotePath
		<< "item: " << currentItem << ", name:" << list.at(currentItem);

	emit getFileObjectIdFailed();
}

//----------------------------------------------------------------------------

GetChildIdResourceRef GetChildIdResource::create()
{
	GetChildIdResourceRef resource =
		RestResource::create<GetChildIdResource>();

	QLOG_TRACE() << "GetChildIdResource created:" << resource.data();

	return resource;
}


void GetChildIdResource::getChildId(int parentId,
									const QString& fileObjectName)
{
	getChildId(QString::number(parentId), fileObjectName);
}

void GetChildIdResource::getChildId(const QString& parentId,
									const QString& fileObjectName)
{
	QLOG_INFO() << "getChildId() parentId: "
		<< parentId << ", name: " << fileObjectName;

	this->parentId = parentId;
	this->fileObjectName = fileObjectName;

	HeaderList headers;
	QByteArray data;

	doOperation(QNetworkAccessManager::GetOperation, data, headers);
}

QString GetChildIdResource::path() const
{
	return QString("/api/v1/files/getChildren/%1").arg(parentId);
}

QString GetChildIdResource::service() const
{
	return FILES_SERVICE_NAME;
}

bool GetChildIdResource::restricted() const
{
	return true;
}

bool GetChildIdResource::processGetResponse(int status,
											const QByteArray& data,
											const HeaderList&)
{
//	QLOG_TRACE() << "get child id get: " << status << "."
//		<< headers << data;

	if (status != 200)
	{
		QLOG_ERROR() << "GetChildIdResource failed to getChildren";
		emit failed();
		return true;
	}

	QString json = getDataFromJson(data);
	QJsonDocument jsonDoc = QJsonDocument::fromJson(json.toUtf8());

	QString parsingError = "Failed to parse getChildren JSON";
	QString searchError =
		"Failed to find " + fileObjectName + " in " + parentId;

	if (!jsonDoc.isArray())
	{
		QLOG_ERROR() << parsingError;
		emit failed();
	}

	QJsonArray jsonArray = jsonDoc.array();

	int id = 0;

	for (int i = 0; i < jsonArray.size(); i++)
	{
		QJsonValue value = jsonArray.at(i);
		if (value.type() == QJsonValue::Object)
		{
			QJsonObject obj = value.toObject();
			RemoteFileDesc remoteFileDesc(RemoteFileDesc::fromJson(obj));

			if (remoteFileDesc.isValid())
			{
//				QLOG_TRACE() << "!!!fileObjectName: " << fileObjectName;
//				remoteFileDesc.log();
				if (remoteFileDesc.name == fileObjectName)
				{
					id = remoteFileDesc.id;
				}
			}
		}
	}

	if (id)
	{
		QLOG_TRACE() << this << "GetChildIdResource id found:" << id;
		emit succeeded(id);
	}
	else
	{
		QLOG_ERROR() << searchError;
		emit failed();
	}

	return true;
}

//----------------------------------------------------------------------------

MoveRestResourceRef MoveRestResource::create()
{
	MoveRestResourceRef resource = RestResource::create<MoveRestResource>();
	return resource;
}

void MoveRestResource::move(int fileObjectId,
							const QString& newName,
							int newParentId)
{
	this->fileObjectId = fileObjectId;
	this->newParentId = newParentId;

	QJsonObject fileItem;
	fileItem.insert("id", fileObjectId);
	fileItem.insert("name", newName);
	QJsonArray filesArray;
	filesArray.append(fileItem);
	QJsonObject dataObject;
	dataObject.insert("target_id", newParentId);
	dataObject.insert("files", filesArray);
	QJsonDocument doc(dataObject);
	QLOG_TRACE() << "encoded params as JSON: "
		<< doc.toJson(QJsonDocument::Compact);

	HeaderList headers;
	ParamList params;

	params.append(ParamPair("data", doc.toJson(QJsonDocument::Compact)));

	doOperation(QNetworkAccessManager::PostOperation, params, HeaderList());
}

QString MoveRestResource::path() const
{
	return "/api/v1/files/move";
}

QString MoveRestResource::service() const
{
	return FILES_SERVICE_NAME;
}

bool MoveRestResource::restricted() const
{
	return true;
}

bool MoveRestResource::processPostResponse(int status,
											const QByteArray& data,
											const HeaderList& headers)
{
	QLOG_TRACE() << "move post: " << status << "." << headers << data;

	if (status == 200)
	{
		emit succeeded();
	}
	else
	{
		emit failed(QString::number(status).append(": ").append(data));
	}

	return true;
}

//----------------------------------------------------------------------------

FilesRestResourceRef FilesRestResource::create()
{
	FilesRestResourceRef resource = RestResource::create<FilesRestResource>();
	resource->isDeleteRequest = false;
	return resource;
}

void FilesRestResource::rename(int id, const QString& newName)
{
	this->id = id;

	HeaderList headers;
	headers.append(HeaderPair(RestResource::contentTypeHeader
			, "application/x-www-form-urlencoded; charset=UTF-8"));

	ParamList params;
	params.append(ParamPair("name", newName.toUtf8()));
	isDeleteRequest = false;
	doOperation(QNetworkAccessManager::PutOperation,
		params, "data", headers);
}

void FilesRestResource::remove(int id)
{
	this->id = id;

	QJsonArray filesArray;
	filesArray.append(id);
	QJsonObject dataObject;
	dataObject.insert("ids", filesArray);
	QJsonDocument doc(dataObject);
	QLOG_TRACE() << "encoded params as JSON: "
		<< doc.toJson(QJsonDocument::Compact);

	HeaderList headers;

	headers.append(HeaderPair(RestResource::contentTypeHeader
		, "application/x-www-form-urlencoded; charset=UTF-8"));

	ParamList params;

	params.append(ParamPair("data",
		doc.toJson(QJsonDocument::Compact)));

	//doOperation(QNetworkAccessManager::DeleteOperation, params, HeaderList());

	QString bodyData = QString("{\"ids\":[%1]}").arg(id);
	QByteArray body = QString("data=").toLatin1().append(QUrl::toPercentEncoding(bodyData));

	isDeleteRequest = true;
	doOperation(QNetworkAccessManager::DeleteOperation, body, headers);

}

void FilesRestResource::getFileObject(int id)
{
	this->id = id;
	isDeleteRequest = false;
	doOperation(QNetworkAccessManager::GetOperation,
		QByteArray(), HeaderList());
}

QString FilesRestResource::path() const
{
	return isDeleteRequest
		? QString("/api/v1/files")
		: QString("/api/v1/files/%1").arg(id);
}

QString FilesRestResource::service() const
{
	return FILES_SERVICE_NAME;
}

bool FilesRestResource::restricted() const
{
	return true;
}

bool FilesRestResource::processPutResponse(int status,
		const QByteArray& data, const HeaderList&)
{
	if (status != 200)
	{
		Q_EMIT failed(QString::number(status).append(": ").append(data));
	}

	const QJsonDocument doc =
		QJsonDocument::fromJson(this->getDataFromJson(data).toUtf8());

	const RemoteFileDesc fileDesc = RemoteFileDesc::fromJson(doc.object());

	Q_EMIT succeeded(fileDesc);

	return true;
}

bool FilesRestResource::processGetResponse(int status,
		const QByteArray& data, const HeaderList&)
{
	if (status != 200)
	{
		Q_EMIT failed(QString::number(status).append(": ").append(data));
	}

	const QJsonDocument doc =
		QJsonDocument::fromJson(this->getDataFromJson(data).toUtf8());

	const RemoteFileDesc fileDesc = RemoteFileDesc::fromJson(doc.object());

	Q_EMIT succeeded(fileDesc);

	return true;
}

bool FilesRestResource::processDelResponse(int status,
		const QByteArray& data, const HeaderList&)
{
	if (status != 200)
	{
		Q_EMIT failed(QString::number(status).append(": ").append(data));
	}

	const QJsonDocument doc =
		QJsonDocument::fromJson(this->getDataFromJson(data).toUtf8());

	const RemoteFileDesc fileDesc = RemoteFileDesc::fromJson(doc.object());

	Q_EMIT succeeded(fileDesc);


	return true;
}

//----------------------------------------------------------------------------

TrashRestResourceRef TrashRestResource::create()
{
	TrashRestResourceRef resource = RestResource::create<TrashRestResource>();
	return resource;
}

void TrashRestResource::trash(int fileObjectId)
{
//	QJsonObject fileItem;
//	fileItem.insert("id", fileObjectId);
//	fileItem.insert("name", newName);

	QJsonArray filesArray;
	filesArray.append(fileObjectId);
	QJsonObject dataObject;
	dataObject.insert("ids", filesArray);
	QJsonDocument doc(dataObject);
	QLOG_TRACE() << "encoded params as JSON: "
		<< doc.toJson(QJsonDocument::Compact);

	HeaderList headers;
	ParamList params;

	params.append(ParamPair("data", doc.toJson(QJsonDocument::Compact)));

	doOperation(QNetworkAccessManager::PostOperation, params, HeaderList());
}

QString TrashRestResource::path() const
{
	return "/api/v1/files/trash";
}

QString TrashRestResource::service() const
{
	return FILES_SERVICE_NAME;
}

bool TrashRestResource::restricted() const
{
	return true;
}

bool TrashRestResource::processPostResponse(int status,
										const QByteArray& data,
										const HeaderList& headers)
{
	QLOG_TRACE() << "trash post: " << status << "." << headers << data;

	if (status == 200)
	{
		emit succeeded();
	}
	else
	{
		emit failed(QString::number(status).append(": ").append(data));
	}

	return true;
}


}
