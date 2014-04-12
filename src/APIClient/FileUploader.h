#ifndef FILE_UPLOADER_H
#define FILE_UPLOADER_H

#include "APIClient/ApiTypes.h"

#include <QtNetwork/QNetworkReply>

//#define UPLOAD_URL "http://files.assistent.th/api/v1/content/create"
//#define UPLOAD_URL "http://disk.new.assistent.by/api/v1/content/create"
//#define UPLOAD_URL "http://c0-0-40.files.assistent.th/api/v1/content/create"

// FIXME: use FilesService to build url
#define UPLOAD_URL "http://mts.turbocloud.by/api/v1/content/create"


class QNetworkAccessManager;
class QByteArray;
class QFileInfo;
class QFile;

namespace Drive
{

class FileUploader : public QObject
{
	Q_OBJECT
public:

	FileUploader(QObject *parent = 0);
	~FileUploader();

	void uploadFile(int parentFolderId, const QString &filePath);

signals:
	void succeeded(Drive::RemoteFileDesc fileDesc);
	void failed(const QString& error);

private slots:
	void onFinished();
	void onError(QNetworkReply::NetworkError code);
	void uploadProgress(qint64 bytesSent, qint64 bytesTotal);


private:
	// created body for POST request with
	QByteArray createPostBody(int workspaceId, int parentFolderId, const QFileInfo &fileInfo);

	QFile *file;
	QString authToken;
	QNetworkAccessManager *man;
	QNetworkReply *reply;

};

}


#endif // FILE_UPLOADER_H
