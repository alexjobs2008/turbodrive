#ifndef FILE_DOWNLOADER_H
#define FILE_DOWNLOADER_H

#include "watchdog.h"

#include <QtNetwork/QNetworkReply>
#include <QtCore/QElapsedTimer>
#include <QtCore/QTime>
#include <QtCore/QSet>

//#define DISK_DOWNLOAD_URL "http://files.assistent.th/api/v1/content/%1"
//#define DISK_DOWNLOAD_URL "http://disk.new.assistent.by/api/v1/content/%1"
//#define DISK_DOWNLOAD_URL "http://c0-0-40.files.assistent.th/api/v1/content/%1"

// FIXME: use FilesService to build url
#define DISK_DOWNLOAD_URL "http://disk.mts.by/api/v1/content/%1"

class QNetworkAccessManager;
class QFile;
class QByteArray;

namespace Drive
{

class FileDownloader : public QObject
{
	Q_OBJECT
public:
	FileDownloader(int fileId, const QString& localPath,
		uint modifiedAt, QObject *parent = 0);

	void download();
	void limitSpeed(int kbPerSecond);

signals:
	void succeeded();
	void failed(const QString& error);
	void downloadSpeed(int kbPerSecond);

private slots:
	void onError(QNetworkReply::NetworkError error);
	void onReplyFinished();
	void onReadyRead();
	void onSslErrors(QNetworkReply* reply, const QList<QSslError>& errors);

private:
	QNetworkAccessManager *nam;
	QNetworkReply *reply;
	int fileId;
	QString localPath;
	uint modifiedAt;
	QFile* file;
	uint totalSize;
	QElapsedTimer elapsedTimer;
	int speedLimit;
	WatchDog m_watchDog;
};

//
// class FileDownloader : public QObject
// {
//	Q_OBJECT
// public:
//
//	FileUploader(QObject *parent = 0);
//
//	void setAuthToken(const QString &authToken);
//
//	void downloadFile(int fileObjectId);
//
// signals:
//
// private slots:
//	void onFinished();
//	void onError(QNetworkReply::NetworkError code);
//	void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
//
//
// private:
//	// created body for POST request with
//	QByteArray createPostBody(int workspaceId, int parentFolderId, const QFileInfo &fileInfo);
//
//	QString authToken;
//	QNetworkAccessManager *man;
//
// };

}

#endif // FILE_DOWNLOADER_H
