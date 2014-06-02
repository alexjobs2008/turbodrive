#include "chunkuploader.h"

#include "QsLog/QsLog.h"

#include "Application/AppController.h"
#include "Network/RestResource.h"

#include <QtCore/QUuid>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <QtNetwork/QHttpMultiPart>
#include <QtNetwork/QHttpPart>

namespace Drive
{

namespace
{

QNetworkRequest createRequest()
{
	// TODO: FIXME: use FilesService to build url
	static const auto s_uploadUrl =
			QString::fromLatin1("http://disk.mts.by/api/v1/content/create");

	auto request = QNetworkRequest(QUrl(s_uploadUrl));

	request.setRawHeader(RestResource::authTokenHeader,
		AppController::instance().authToken().toUtf8());

	request.setRawHeader(RestResource::workspaceHeader,
		QString::number(AppController::instance()
			.profileData().defaultWorkspace().id).toUtf8());

	return request;
}

QHttpPart createHttpPart(const char* name, const QVariant& body)
{
	static const auto s_header = QString::fromLatin1("form-data; name=\"%1\"");
	QHttpPart httpPart;
	httpPart.setHeader(
			QNetworkRequest::ContentDispositionHeader,
			QVariant(s_header.arg(QLatin1String(name))));
	Q_ASSERT(body.canConvert<QString>());
	httpPart.setBody(body.toString().toUtf8());
	return httpPart;
}

}

ChunkUploader::ChunkUploader(
		QFile& file, const int offset, const int size,
		const int chunkIndex, const int totalChunks,
		const int folderId, QObject* parent)
	: QObject(parent)
	, m_file(file)
	, m_offset(offset)
	, m_size(size)
	, m_chunkIndex(chunkIndex)
	, m_chunksTotal(totalChunks)
	, m_folderId(folderId)
	, m_networkReply(nullptr)
{
	Q_ASSERT(m_file.isOpen());
}

void ChunkUploader::start()
{
	static const auto s_message = QString::fromLatin1(
			"%1 Chunk uploading started.");
	QLOG_INFO() << s_message.arg(state());

	const auto request = createRequest();
	const auto multiPart = createHttpMultiPart();

	m_networkReply = m_network.post(request, multiPart);
	Q_ASSERT(m_networkReply);

	// delete the multiPart with the reply
	multiPart->setParent(m_networkReply);

	connect(m_networkReply, &QNetworkReply::finished,
			this, &ChunkUploader::onFinished);

	connect(m_networkReply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
			this, &ChunkUploader::onError);
}

void ChunkUploader::onFinished()
{
	const auto status = m_networkReply->attribute(
			QNetworkRequest::HttpStatusCodeAttribute).toInt();

	if (status == 200)
	{
		static const auto s_message = QString::fromLatin1(
				"%1 Chunk uploading finished successfully."
				" Network reply data received: '%2'.");
		const QByteArray replyData = m_networkReply->readAll();
		QLOG_INFO() << s_message.arg(state(), QString(replyData));
		Q_EMIT finished(replyData);
	}
	else
	{
		static const auto s_message = QString::fromLatin1(
				"%1 Chunk uploading failed with http status %2.");
		QLOG_ERROR() << s_message.arg(state(), QString::number(status));
		onError(m_networkReply->error());
	}
}

void ChunkUploader::onError(const QNetworkReply::NetworkError code)
{
	static const auto s_message = QString::fromLatin1(
			"%1 Chunk uploading failed with network error %2.");
	QLOG_ERROR() << s_message.arg(state(), QString::number(code));
	Q_EMIT error(code);
}

QByteArray ChunkUploader::read() const
{
	Q_ASSERT(m_file.isOpen());
	Q_ASSERT(m_file.seek(m_offset));
	const QByteArray data = m_file.read(m_size);
	Q_ASSERT(data.size() == m_size);
	return data;
}

QHttpMultiPart* ChunkUploader::createHttpMultiPart() const
{
	static const auto s_folderId = QString::fromLatin1("folderId");
	QJsonObject data;
	data.insert(s_folderId, m_folderId);
	const auto dataJson = QJsonDocument(data).toJson(QJsonDocument::Compact);

	const QFileInfo fileInfo(m_file);

	QHttpMultiPart* multiPart =
		new QHttpMultiPart(QHttpMultiPart::FormDataType);
	multiPart->append(createHttpPart("data", dataJson));
	multiPart->append(createHttpPart("qqpartindex", m_chunkIndex));
	multiPart->append(createHttpPart("qqpartbyteoffset", m_offset));
	multiPart->append(createHttpPart("qqchunksize", m_size));
	multiPart->append(createHttpPart("qqtotalparts", m_chunksTotal));
	multiPart->append(createHttpPart("qqtotalfilesize", m_file.size()));
	multiPart->append(createHttpPart("qqfilename", m_file.fileName()));
	multiPart->append(createHttpPart("qquuid", QUuid::createUuid().toString()));
	multiPart->append(createHttpPart("createdAt", fileInfo.created().toTime_t()));
	multiPart->append(createHttpPart("updatedAt", fileInfo.lastModified().toTime_t()));

	static const auto s_qqfile =
			QString::fromLatin1("form-data; name=\"qqfile\"; filename=\"%1\"");
	QHttpPart qqfilePart;
	qqfilePart.setHeader(QNetworkRequest::ContentDispositionHeader,
			QVariant(s_qqfile.arg(m_file.fileName())));
	qqfilePart.setBody(read());
	multiPart->append(qqfilePart);

	return multiPart;
}

QString ChunkUploader::state() const
{
	static const auto s_message = QString::fromLatin1("[%1:%2:%3]");
	return s_message.arg(
			m_file.fileName(),
			QString::number(m_offset),
			QString::number(m_size));
}


}
