#ifndef FILE_UPLOADER_H
#define FILE_UPLOADER_H

#include <QtNetwork/QNetworkReply>

class QNetworkAccessManager;
class QByteArray;
class QFileInfo;

namespace Drive
{

class FileUploader : public QObject
{
    Q_OBJECT
public:

    FileUploader(QObject *parent = 0);

    void setAuthToken(const QString &authToken);

    void uploadFile(int workspaceId, int parentFolderId,
        const QString &filePath);

signals:
    
private slots:
    void onFinished();
    void onError(QNetworkReply::NetworkError code);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);


private:
    // created body for POST request with 
    QByteArray createPostBody(int workspaceId, int parentFolderId, const QFileInfo &fileInfo);

    QString authToken;
    QNetworkAccessManager *man;

};

}


#endif // FILE_UPLOADER_H