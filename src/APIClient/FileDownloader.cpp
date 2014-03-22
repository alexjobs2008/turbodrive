#include "FileDownloader.h"

#include "Application/AppController.h"
#include "Network/RestResource.h"
#include "Util/FileUtils.h"
#include "QsLog/QsLog.h"

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QThread>
#include <QtNetwork/QSslError>
#include <QtNetwork/QNetworkCookieJar>

namespace Drive
{

FileDownloader::FileDownloader(int fileId, const QString& localPath, uint modifiedAt, QObject *parent)
	: QObject(parent)
	, fileId(fileId)
	, localPath(localPath)
	, modifiedAt(modifiedAt)
	, file(0)
	, timerId(0)
	, speedLimit(0)
{
}

void FileDownloader::download()
{
	file = new QFile(localPath, this);
	bool opened = file->open(QIODevice::WriteOnly);

	if (!opened)
	{
		emit failed(QString(tr("Failed to write to file: %1")).arg(localPath));
		return;
	}

	nam = new QNetworkAccessManager(this);
	QNetworkRequest request(QString(DISK_DOWNLOAD_URL).arg(fileId));

	request.setRawHeader(RestResource::authTokenHeader,
		AppController::instance().authToken().toUtf8());

	request.setRawHeader(RestResource::workspaceHeader,
		QString::number(
		AppController::instance().profileData().defaultWorkspace().id).toUtf8());

	connect(nam, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
		this, SLOT(onSslErrors(QNetworkReply*,const QList<QSslError>&)));

	QLOG_TRACE() << "Downloading file:" << request.url();

	reply = nam->get(request);
	reply->setParent(nam);

	elapsedTimer.start();
	totalSize = 0;

	bool b = connect(reply, SIGNAL(finished()),
		this, SLOT(onReplyFinished()));

	QLOG_TRACE() << b;

	b = connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
		this, SLOT(onError(QNetworkReply::NetworkError)));

	QLOG_TRACE() << b;

	b = connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
		this, SLOT(onDownloadProgress(qint64, qint64)));

	QLOG_TRACE() << b;

	b = connect(reply, SIGNAL(readyRead()),
		this, SLOT(onReadyRead()));

	QLOG_TRACE() <<
		reply->isRunning() << reply->isFinished() << reply->errorString();

//	if (timerId)
//		killTimer(timerId);
//
//	timerId = startTimer(500);
}

void FileDownloader::limitSpeed(int kbPerSecond)
{
	speedLimit = kbPerSecond;
}

void FileDownloader::timerEvent(QTimerEvent * event)
{
//	int speed = totalSize * 1000 / elapsedTimer.elapsed() / 1024;
	//QLOG_TRACE() << "Download speed: " << speed << "Kb/s";
//	emit downloadSpeed(speed);
}

void FileDownloader::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
//	QLOG_TRACE() << "Downloaded" << bytesReceived
//		<< "bytes of " << bytesTotal;
}

void FileDownloader::onReplyFinished()
{
//	int speed = totalSize * 1000 / elapsedTimer.elapsed() / 1024;

//	QLOG_TRACE() << "Download finished (" << totalSize <<
//		"bytes at " << speed << "Kb/s" << "):" << localPath;
//
//	emit downloadSpeed(speed);

	float elapsed = elapsedTimer.elapsed();

	QLOG_TRACE() << "DOWNLOAD FINISHED in " << elapsed << "ms"
		<< ". File:" << localPath << ", size:" << totalSize
		<< ". Speed:" << float(totalSize) * 1000.0 / elapsed / 1024.0 << "Kb/s";

	file->close();
	file->deleteLater();

	FileSystemHelper::setFileModificationTimestamp(localPath, modifiedAt);

	emit succeeded();
}

void FileDownloader::onError(QNetworkReply::NetworkError error)
{
	QLOG_TRACE() << "Network error: " << error;
	emit failed(QString(tr("Network error: %1")).arg(error));
}

void FileDownloader::onReadyRead()
{
	QByteArray data(reply->readAll());
	totalSize += data.size();
	file->write(data);
}

void FileDownloader::onSslErrors(QNetworkReply *reply, const QList<QSslError> & errors)
{
	for (int i = 0; i < errors.size(); i++)
	{
		QSslError error = errors.at(i);
		QLOG_ERROR() << "SSL Error: " << error.errorString();
	}

	if (errors.size())
		QLOG_INFO() << "Ignoring SSL errors.";

	reply->ignoreSslErrors();
}

}