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
    
    connect(reply, SIGNAL(finished()),
        this, SLOT(onReplyFinished()));

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
        this, SLOT(onError(QNetworkReply::NetworkError)));

    connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
        this, SLOT(onDownloadProgress(qint64, qint64)));    
    
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

    if (type == SimpleDownloader::Data)
    {
        emit finished(data);
    }
    else if (type == SimpleDownloader::Pixmap)
    {
        QPixmap pixmap;
        pixmap.loadFromData(data);
        emit finished(pixmap);
    }
}