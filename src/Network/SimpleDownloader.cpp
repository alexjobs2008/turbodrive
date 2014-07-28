#include "SimpleDownloader.h"
#include "QsLog/QsLog.h"

#include <QtCore/QJsonDocument>
#include <QtNetwork/QNetworkAccessManager>
#include <QtGui/QPixmap>

SimpleDownloader::SimpleDownloader(QUrl url, Type type, QObject* parent)
	: QObject(parent)
	, m_type(type)
	, m_url(url.toString())
	, m_networkAccessManager(new QNetworkAccessManager(this))
{

	m_networkReply = m_networkAccessManager->get(QNetworkRequest(url));
	m_networkReply->setParent(m_networkAccessManager);

	connect(m_networkReply, &QNetworkReply::finished,
			this, &SimpleDownloader::onReplyFinished);

	connect(m_networkReply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
		this, &SimpleDownloader::error);
}

void SimpleDownloader::onReplyFinished()
{
	const QByteArray data = m_networkReply->readAll();
	QLOG_INFO() << "Download finished: " << data.size()
		<< "bytes. " << m_url;

	if (m_type == SimpleDownloader::Data)
	{
		Q_EMIT finished(data);
	}
	else
	{
		QPixmap pixmap;
		pixmap.loadFromData(data);
		Q_EMIT pixmapDownloaded(pixmap);
	}
}

