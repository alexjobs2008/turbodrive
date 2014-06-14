#ifndef FILES_SERVICE_H
#define FILES_SERVICE_H

#include "Network/RestResource.h"
#include "ApiTypes.h"

#include <QtCore/QVariantMap>
#include <QtCore/QStringList>

class QJsonObject;

namespace Drive
{

class OnlineRestResource;
typedef QSharedPointer<OnlineRestResource> OnlineRestResourceRef;

class OnlineRestResource : public RestResource
{
	Q_OBJECT
public:

	static OnlineRestResourceRef create();

	void ping();

	virtual QString path() const;
	virtual QString service() const;
	virtual bool restricted() const;

signals:
	void pingOk();
	void pingError();

private:
	virtual bool processGetResponse(int status, const QByteArray&, const HeaderList&);
};

class GetAncestorsRestResource;
typedef QSharedPointer<GetAncestorsRestResource> GetAncestorsRestResourceRef;

class GetAncestorsRestResource : public RestResource
{
	Q_OBJECT
public:
	static GetAncestorsRestResourceRef create();

	void getAncestors(int fileObjectId);

	virtual QString path() const;
	virtual QString service() const;
	virtual bool restricted() const;

signals:
	void succeeded(const QString& fullPath);
	void failed();

private:
	virtual bool processGetResponse(int status, const QByteArray& data, const HeaderList&);

	int fileId;
};

class ContentRestResource;
typedef QSharedPointer<ContentRestResource> ContentRestResourceRef;

class ContentRestResource : public RestResource
{
	Q_OBJECT
public:
	static ContentRestResourceRef create();

	void download(int fileId);

	virtual QString path() const;
	virtual QString service() const;
	virtual bool restricted() const;

signals:
	void succeeded(const QString& fullPath);
	void failed();

private:
	virtual bool processGetResponse(int status, const QByteArray& data,
		const HeaderList& headers);

	int fileId;
};

class CreateRestResource;
typedef QSharedPointer<CreateRestResource> CreateRestResourceRef;

class CreateRestResource : public RestResource
{
	Q_OBJECT
public:
	static CreateRestResourceRef create();

	void createFolder(int parentId, const QString& folderName);

	virtual QString path() const;
	virtual QString service() const;
	virtual bool restricted() const;

signals:
	void succeeded(Drive::RemoteFileDesc fileDesc);
	void failed(const QString& error);

private:
	virtual bool processPostResponse(int status, const QByteArray& data, const HeaderList&);

	int parentId;
};

class GetChildrenResource;
typedef QSharedPointer<GetChildrenResource> GetChildrenResourceRef;

class GetChildIdResource;
typedef QSharedPointer<GetChildIdResource> GetChildIdResourceRef;

class GetChildrenResource : public RestResource
{
	Q_OBJECT
public:
	static GetChildrenResourceRef create();

	void getChildren(const QString& parentId);
	void getChildren(int parentId);

	void getFileObjectId(const QString& remotePath);
	void getFileObjectParentId(const QString& remotePath);

	virtual QString path() const;
	virtual QString service() const;
	virtual bool restricted() const;

signals:
	void succeeded(QList<Drive::RemoteFileDesc> list);
	void failed();
	void getFileObjectIdSucceeded(int id);
	void getFileObjectIdFailed();

private slots:
	void onGetChildIdSucceeded(int id);
	void onGetChildIdFailed();

private:
	void getFirstChildId();
	void getNextChildId(int parentId);
	virtual bool processGetResponse(int status, const QByteArray& data,
		const HeaderList& headers);


	QString remotePath;
	QString parentId;
	QStringList list;
	int currentItem;
	GetChildIdResourceRef res;
};

class GetChildIdResource : public RestResource
{
	Q_OBJECT
public:
	static GetChildIdResourceRef create();

	void getChildId(const QString& parentId, const QString& fileObjectName);
	void getChildId(int parentId, const QString& fileObjectName);

	virtual QString path() const;
	virtual QString service() const;
	virtual bool restricted() const;

signals:
	void succeeded(int id);
	void failed();

private:
	virtual bool processGetResponse(int status, const QByteArray& data, const HeaderList&);

	QString parentId;
	QString fileObjectName;
};

class MoveRestResource;
typedef QSharedPointer<MoveRestResource> MoveRestResourceRef;

class MoveRestResource : public RestResource
{
	Q_OBJECT
public:
	static MoveRestResourceRef create();

	void move(int fileObjectId, const QString& newName, int newParentId);

	virtual QString path() const;
	virtual QString service() const;
	virtual bool restricted() const;

signals:
	void succeeded();
	void failed(const QString& error);

private:
	virtual bool processPostResponse(int status, const QByteArray& data,
		const HeaderList& headers);

	int fileObjectId;
	int newParentId;
};

class FilesRestResource;
typedef QSharedPointer<FilesRestResource> FilesRestResourceRef;

class FilesRestResource : public RestResource
{
	Q_OBJECT
public:
	static FilesRestResourceRef create();

	void rename(int id, const QString& newName);
	void remove(int id);
	void getFileObject(int id);

	virtual QString path() const;
	virtual QString service() const;
	virtual bool restricted() const;

signals:
	void succeeded(const Drive::RemoteFileDesc& fileDesc);
	void failed(const QString& error);

private:
	virtual bool processPutResponse(int status,
			const QByteArray& data, const HeaderList&);
	virtual bool processGetResponse(int status,
			const QByteArray& data, const HeaderList&);
	virtual bool processDelResponse(int status,
			const QByteArray& data, const HeaderList&);

	int id;
	bool isDeleteRequest;
};

class TrashRestResource;
typedef QSharedPointer<TrashRestResource> TrashRestResourceRef;

class TrashRestResource: public RestResource
{
	Q_OBJECT
public:
	static TrashRestResourceRef create();

	void trash(int fileObjectId);

	virtual QString path() const;
	virtual QString service() const;
	virtual bool restricted() const;

signals:
	void succeeded();
	void failed(const QString& error);

private:
	virtual bool processPostResponse(int status, const QByteArray& data,
		const HeaderList& headers);
};

}

#endif // FILES_SERVICE_H
