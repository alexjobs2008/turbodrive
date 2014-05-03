#include "SimpleDownloader.h"
#include "QsLog/QsLog.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtGui/QPixmap>

SimpleDownloader::SimpleDownloader(QUrl url, Type type, QObject *parent)
	: QObject(parent)
	, type(type)
{
	nam = new QNetworkAccessManager(this);
	QNetworkRequest request(url);

	reply = nam->get(request);
	reply->setParent(nam);

	connect(reply, &QNetworkReply::finished,
			this, &SimpleDownloader::onReplyFinished);

	connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
		this, &SimpleDownloader::onError);

	connect(reply, &QNetworkReply::downloadProgress,
		this, &SimpleDownloader::onDownloadProgress);

	urlString = url.toString();
}

void SimpleDownloader::onDownloadProgress(qint64 bytesReceived,
										qint64 bytesTotal)
{
	QLOG_TRACE() << "received" << bytesReceived << "bytes of " << bytesTotal
		<< "from" << urlString;
}

void SimpleDownloader::onError(QNetworkReply::NetworkError error)
{
	QLOG_ERROR() << "Error downloading" << urlString << ": " << error;
}

void SimpleDownloader::onReplyFinished()
{
	QByteArray data = reply->readAll();
	QLOG_INFO() << "Download finished: " << data.size()
		<< "bytes. " << urlString;

	if (type == SimpleDownloader::Pixmap)
	{
		QPixmap pixmap;
		pixmap.loadFromData(data);
		emit finished(pixmap);
	}
}
