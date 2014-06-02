#ifndef CHUNK_UPLOADER_H
#define CHUNK_UPLOADER_H

#include "APIClient/ApiTypes.h"

#include <QtCore/QFile>
#include <QtNetwork/QNetworkReply>

namespace Drive
{

class ChunkUploader: public QObject
{
	Q_OBJECT

public:
	ChunkUploader(const QString& uuid,
			QFile& file, const int offset, const int size,
			const int chunkIndex, const int totalChunks,
			const int folderId, QObject* parent = nullptr);

	Q_SLOT void start();

	Q_SIGNAL void finished(const QByteArray& replyData);
	Q_SIGNAL void error(QNetworkReply::NetworkError);

public:
	Q_SLOT void onFinished();
	Q_SLOT void onError(const QNetworkReply::NetworkError code);

private:
	QByteArray read() const;
	QHttpMultiPart* createHttpMultiPart() const;
	QString state() const;

private:
	const QString m_uuid;

	QFile& m_file;
	const int m_offset;
	const int m_size;
	const int m_folderId;

	const int m_chunkIndex;
	const int m_chunksTotal;

	QNetworkAccessManager m_network;
	QNetworkReply* m_networkReply;
};

}

#endif // CHUNK_UPLOADER_H
