#include "FileUploader.h"

#include "chunkuploader.h"

#include "QsLog/QsLog.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace Drive
{

FileUploader::FileUploader(const int folderId, const QString& filePath, QObject* parent)
	: QObject(parent)
{
	auto file = new QFile(filePath, this);
	Q_ASSERT(file->exists());

	// FIXME: get rid of this terrible solution
	while(!file->open(QIODevice::ReadOnly))
	{
		QLOG_ERROR() << file->errorString();
	}

	static const qint64 s_maxChunkSize = 2048 * 1024;

	const int chunksTotal = std::ceil(file->size() / s_maxChunkSize);
	std::vector<ChunkUploader*> uploaders;

	for (int chunkIndex = 0, offset = 0;
			offset < file->size();
			++chunkIndex, offset += s_maxChunkSize)
	{
		const int size = std::min(file->size() - offset, s_maxChunkSize);
		auto nextUploader = new ChunkUploader(*file, offset, size,
				chunkIndex, chunksTotal, folderId, this);
		if (!uploaders.empty())
		{
			connect(uploaders.back(), &ChunkUploader::finished,
					nextUploader, &ChunkUploader::start);
		}
		connect(nextUploader, &ChunkUploader::error,
				this, &FileUploader::onError);
		uploaders.push_back(nextUploader);
	}

	Q_ASSERT(!uploaders.empty());
	connect(uploaders.back(), &ChunkUploader::finished,
			this, &FileUploader::onFinished);

	uploaders.front()->start();
}

void FileUploader::onFinished(const QByteArray& data)
{
	QJsonDocument doc = QJsonDocument::fromJson(data);

	Q_ASSERT(doc.isObject());
	const QJsonObject replyObj = doc.object();

	Q_ASSERT(replyObj.contains("data"));
	const QJsonValue dataValue = replyObj.value("data");

	Q_ASSERT(dataValue.isObject());
	const QJsonObject dataObj = dataValue.toObject();

	Q_EMIT succeeded(RemoteFileDesc::fromJson(dataObj));
}

void FileUploader::onError(QNetworkReply::NetworkError code)
{
	QLOG_ERROR() << "Uploader error:" << code;
	Q_EMIT failed(QString::number(code));
}

}
