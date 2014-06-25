#include "FileUploader.h"

#include "chunkuploader.h"

#include "QsLog/QsLog.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUuid>

#include <cmath>

namespace Drive
{

FileUploader::FileUploader(const int folderId, const QString& filePath, QObject* parent)
	: QObject(parent)
	, m_file(new QFile(filePath, this))
{
	Q_ASSERT(m_file->exists());

	// FIXME: get rid of this terrible solution
	while(!m_file->open(QIODevice::ReadOnly))
	{
		QLOG_ERROR() << m_file->errorString();
	}

	static const qint64 s_maxChunkSize = 2048 * 1024;

	const QString uuid = QUuid::createUuid().toString();
	const int chunksTotal = std::ceil(m_file->size() / s_maxChunkSize);
	std::vector<ChunkUploader*> uploaders;

	for (int chunkIndex = 0, offset = 0;;)
	{
		const int size = std::min(m_file->size() - offset, s_maxChunkSize);
		auto nextUploader = new ChunkUploader(uuid, *m_file, offset, size,
				chunkIndex, chunksTotal, folderId, this);
		if (!uploaders.empty())
		{
			connect(uploaders.back(), &ChunkUploader::finished,
					nextUploader, &ChunkUploader::start);
		}
		connect(nextUploader, &ChunkUploader::error,
				this, &FileUploader::onError);
		uploaders.push_back(nextUploader);

		++chunkIndex;
		offset += s_maxChunkSize;
		if (offset >= m_file->size())
		{
			break;
		}
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

	if (m_file)
	{
		m_file->close();
	}
}

void FileUploader::onError(QNetworkReply::NetworkError code)
{
	QLOG_ERROR() << "Uploader error:" << code;
	Q_EMIT failed(QString::number(code));

	if (m_file)
	{
		m_file->close();
	}
}

}
