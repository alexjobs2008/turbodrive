#include "FileUploader.h"

#include "QsLog/QsLog.h"

#include <QtCore/QList>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QUuid>
#include <QtCore/QDateTime>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QHttpMultiPart>


namespace Drive
{

FileUploader::FileUploader(QObject *parent)
    : QObject(parent)
    , authToken(QString())
{
    man = new QNetworkAccessManager(this);
}

void FileUploader::setAuthToken(const QString &authToken)
{
    this->authToken = authToken;
}

void FileUploader::uploadFile(int workspaceId, int parentFolderId,
                              const QString &filePath)
{
    if (authToken.isEmpty())
    {
        QLOG_ERROR() << "Upload Error. Auth token is not set";
        return;
    }
    
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists())
    {
        QLOG_ERROR() << "Upload Error. File does not exist: " << filePath;
        return;
    }

    QFile *file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly))
    {
        QLOG_ERROR() << "Upload Error. Unable to open file: " << filePath;
        return;
    }
    
    QUrl url("https://files.assistent.by/api/v1/content");
    QNetworkRequest request(url);

    request.setRawHeader("X-Sso-Token", authToken.toUtf8());
    request.setRawHeader("X-Current-Workspace-Id",
        QString::number(workspaceId).toUtf8());

    // data = { "folderId" : NN }
    QVariantMap map;
    map.insert("folderId", parentFolderId);
    QJsonObject jobject = QJsonObject::fromVariantMap(map);
    QJsonDocument doc(jobject);

    typedef QPair<QString, QString> StringPair;    
    QList<StringPair> list;
    list << StringPair("data", doc.toJson(QJsonDocument::Compact));
    list << StringPair("qqpartindex", "0");
    list << StringPair("qqpartbyteoffset", "0");
    list << StringPair("qqchunksize", QString("%1").arg(fileInfo.size()));
    list << StringPair("qqtotalparts", "1");
    list << StringPair("qqtotalfilesize", QString("%1").arg(fileInfo.size()));
    list << StringPair("qqfilename", fileInfo.fileName());
    list << StringPair("qquuid", QUuid::createUuid().toString());
    list << StringPair("createdAt",
        QString::number(QDateTime::currentDateTimeUtc().toTime_t()));
    list << StringPair("updatedAt",
        QString::number(QDateTime::currentDateTimeUtc().toTime_t()));


    QHttpMultiPart *multiPart =
        new QHttpMultiPart(QHttpMultiPart::FormDataType);

    while (!list.isEmpty())
    {
        StringPair pair = list.takeFirst();
        QHttpPart httpPart;
        httpPart.setHeader(QNetworkRequest::ContentDispositionHeader,
            QVariant(QString("form-data; name=\"%1\"").arg(pair.first)));
        
        httpPart.setBody(pair.second.toUtf8().toPercentEncoding());

        multiPart->append(httpPart);
    }

    QHttpPart qqfilePart;
    qqfilePart.setHeader(QNetworkRequest::ContentDispositionHeader
        , QVariant(QString("form-data; name=\"qqfile\"; filename=\"%1\"")
        .arg(fileInfo.fileName())));
     
    qqfilePart.setBodyDevice(file);     
     
    file->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart 

    QNetworkReply *reply = man->post(request, multiPart);
    multiPart->setParent(reply); // delete the multiPart with the reply

    connect(reply, SIGNAL(finished()),
        this, SLOT(onFinished()));

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
        this, SLOT(onError(QNetworkReply::NetworkError)));

    connect(reply, SIGNAL(uploadProgress(qint64, qint64)),
        this, SLOT(uploadProgress(qint64, qint64)));
}

void FileUploader::onFinished()
{
    QLOG_INFO() << "Uploader: finished.";
}

void FileUploader::onError(QNetworkReply::NetworkError code)
{
    QLOG_INFO() << "Uploader error:" << code;
}

void FileUploader::uploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    QLOG_INFO() << "Upload progress: " << bytesSent << "of" << bytesTotal;
}

}