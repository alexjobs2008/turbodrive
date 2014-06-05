#ifndef FILE_UPLOADER_H
#define FILE_UPLOADER_H

#include "APIClient/ApiTypes.h"

#include <QtNetwork/QNetworkReply>

class QFile;

namespace Drive
{

class FileUploader : public QObject
{
	Q_OBJECT

public:
	FileUploader(int folderId, const QString& filePath, QObject* parent = nullptr);

	Q_SIGNAL void succeeded(Drive::RemoteFileDesc fileDesc);
	Q_SIGNAL void failed(const QString& error);

private:
	Q_SLOT void onFinished(const QByteArray& data);
	Q_SLOT void onError(QNetworkReply::NetworkError code);

private:
	QFile* m_file;
};

}

#endif // FILE_UPLOADER_H
