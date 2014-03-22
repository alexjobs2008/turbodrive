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

    explicit SimpleDownloader(QUrl url, Type type = Pixmap,
        QObject *parent = 0);

signals:
    void finished(const QByteArray& data);
    void finished(const QPixmap& pixmap);

private slots:
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onError(QNetworkReply::NetworkError error);
    void onReplyFinished();

private:
    Type type;
    QNetworkAccessManager *nam;
    QNetworkReply *reply;
    QString urlString;
};

#endif // SIMPLE_DOWNLOADER_H