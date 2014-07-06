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
	, speedLimit(0)
	, m_watchDog([this] { QLOG_ERROR() << "Connection has been lost."; reply->abort(); })
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

	connect(nam, &QNetworkAccessManager::sslErrors,
			this, &FileDownloader::onSslErrors);

	QLOG_TRACE() << "Downloading file:" << request.url();

	reply = nam->get(request);
	reply->setParent(nam);

	elapsedTimer.start();
	totalSize = 0;

	Q_ASSERT(connect(reply, &QNetworkReply::finished,
			this, &FileDownloader::onReplyFinished));

	Q_ASSERT(connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
			this, &FileDownloader::onError));

	Q_ASSERT(connect(reply, &QNetworkReply::readyRead,
			this, &FileDownloader::onReadyRead));

	Q_ASSERT(connect(reply, &QNetworkReply::finished,
			&m_watchDog, &WatchDog::stop));

	Q_ASSERT(connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
			&m_watchDog, &WatchDog::stop));

	Q_ASSERT(connect(reply, &QNetworkReply::downloadProgress,
			&m_watchDog, &WatchDog::restart));

	m_watchDog.restart();
}

void FileDownloader::limitSpeed(int kbPerSecond)
{
	speedLimit = kbPerSecond;
}

void FileDownloader::onReplyFinished()
{
	const auto elapsed = elapsedTimer.elapsed();

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
	if (!file->remove())
	{
		static const auto s_message = QString::fromUtf8("File removing failed: %1.");
		QLOG_ERROR() << s_message.arg(file->fileName());
	}

	QLOG_TRACE() << "Network error: " << error;
	emit failed(QString(tr("Network error: %1")).arg(error));
}

void FileDownloader::onReadyRead()
{
	QByteArray data(reply->readAll());
	totalSize += data.size();
	file->write(data);
}

void FileDownloader::onSslErrors(QNetworkReply* reply, const QList<QSslError>& errors)
{
	for (int i = 0; i < errors.size(); i++)
	{
		QSslError error = errors.at(i);
		QLOG_ERROR() << "SSL Error: " << error.errorString();
	}

	if (errors.size())
	{
		QLOG_INFO() << "Ignoring SSL errors.";
	}

	reply->ignoreSslErrors();
}

}
