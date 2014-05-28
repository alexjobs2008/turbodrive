#ifndef SIMPLE_DOWNLOADER_H
#define SIMPLE_DOWNLOADER_H

#include <QObject>
#include <QByteArray>

#include <QtNetwork/QNetworkReply>

class QNetworkAccessManager;

class SimpleDownloader : public QObject
{
	Q_OBJECT

public:
	enum Type
	{
		Data = 0,
		Pixmap
	};

	explicit SimpleDownloader(QUrl url,
		Type m_type = Pixmap, QObject* parent = nullptr);

	Q_SIGNAL void finished(const QByteArray& data);

	Q_SIGNAL void pixmapDownloaded(const QPixmap& pixmap);

private slots:
	void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	void onError(QNetworkReply::NetworkError error);
	void onReplyFinished();

private:
	const Type m_type;
	const QString m_url;

	QNetworkAccessManager* m_networkAccessManager;
	QNetworkReply* m_networkReply;
};

#endif // SIMPLE_DOWNLOADER_H
