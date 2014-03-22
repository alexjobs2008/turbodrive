#include "FileUploader.h"

#include "QsLog/QsLog.h"
#include "Application/AppController.h"
#include "Network/RestResource.h"

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

FileUploader::~FileUploader()
{
    QLOG_TRACE() << "FileUploader::~FileUploader()";
}

void FileUploader::uploadFile(int parentFolderId, const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists())
    {
        emit failed("Upload Error. File does not exist: " + filePath);
        return;
    }

    file = new QFile(filePath, this);

    if (!file->open(QIODevice::ReadOnly))
    {
        emit failed("Upload Error. Unable to open file: " + filePath);
        return;
    }
    
    QUrl url(UPLOAD_URL);
    
    QNetworkRequest request(url);

    request.setRawHeader(RestResource::authTokenHeader,
        AppController::instance().authToken().toUtf8());

    request.setRawHeader(RestResource::workspaceHeader,
        QString::number(AppController::instance()
            .profileData().defaultWorkspace().id).toUtf8());

    // data = { "folderId" : NN }
    QVariantMap map;
    map.insert("folderId", parentFolderId);
    QJsonObject jobject = QJsonObject::fromVariantMap(map);
    QJsonDocument doc(jobject);

    QLOG_TRACE() << "data:" << doc.toJson(QJsonDocument::Compact);
    
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
        QString::number(fileInfo.created().toTime_t()));
    list << StringPair("updatedAt",
        QString::number(fileInfo.lastModified().toTime_t()));

    QHttpMultiPart *multiPart =
        new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QLOG_TRACE() << multiPart->boundary();

    while (!list.isEmpty())
    {
        StringPair pair = list.takeFirst();
        QHttpPart httpPart;
        httpPart.setHeader(QNetworkRequest::ContentDispositionHeader,
            QVariant(QString("form-data; name=\"%1\"").arg(pair.first)));
        
        httpPart.setBody(pair.second.toUtf8());

        multiPart->append(httpPart);
    }

    QHttpPart qqfilePart;
    qqfilePart.setHeader(QNetworkRequest::ContentDispositionHeader
        , QVariant(QString("form-data; name=\"qqfile\"; filename=\"%1\"")
        .arg(fileInfo.fileName())));
     
    qqfilePart.setBodyDevice(file);

    multiPart->append(qqfilePart);
     
    // file->setParent(multiPart);

    reply = man->post(request, multiPart);

    QLOG_TRACE() << request.rawHeaderList();
    QLOG_TRACE() << multiPart->children();

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
    int status =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    
    QString replyBody(reply->readAll());

    QLOG_TRACE() << "Uploader: reply finished:"
        << status
        << reply->url().toString()
        << replyBody;

    RemoteFileDesc fileDesc;
    fileDesc.id = 0;
    
    if (status == 200)
    {
        QJsonDocument doc = QJsonDocument::fromJson(replyBody.toUtf8());
        if (doc.isObject())
        {
            QJsonObject replyObj = doc.object();
            if (replyObj.contains("data"))
            {
                QJsonValue dataValue = replyObj.value("data");
                if (dataValue.isObject())
                {
                    QJsonObject dataObj = dataValue.toObject();
                    fileDesc = RemoteFileDesc::fromJson(dataObj);
                }
            }
        }
        
        emit succeeded(fileDesc);
    }
    else
    {
        emit failed("File upload failed " + QString::number(status)
            + ", " + QString::number(reply->error()));
    }

    if (file)
    {
        file->close();
    }
}

void FileUploader::onError(QNetworkReply::NetworkError code)
{
    QLOG_INFO() << "Uploader error:" << code;
    emit failed(QString::number(code));
    
    if (file)
    {
        file->close();
    }
}

void FileUploader::uploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    QLOG_INFO() << "Upload progress: " << bytesSent << "of" << bytesTotal;
}

}