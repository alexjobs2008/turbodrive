#include "remoteconfig.h"

#include "SimpleDownloader.h"
#include "RestService.h"

#include <QtCore>

namespace Drive
{

RemoteConfig::RemoteConfig(const QString& url, QObject* parent)
	: QObject(parent)
	, m_downloader(new SimpleDownloader(url, SimpleDownloader::Data, this))
{
	connect(m_downloader, &SimpleDownloader::finished,
			this, &RemoteConfig::onDownloadFinished);
}

void RemoteConfig::onDownloadFinished(const QByteArray& data)
{
	QJsonParseError error;

	auto doc = QJsonDocument::fromJson(data, &error);
	Q_ASSERT(error.error == QJsonParseError::NoError);

	static const auto dataKey = QString::fromLatin1("data");
	static const auto environmentKey = QString::fromLatin1("environment");
	static const auto servicesKey = QString::fromLatin1("services");

	const QVariantMap servicesMap = doc.object()
			.value(dataKey).toObject()
			.value(environmentKey).toObject()
			.value(servicesKey).toObject().toVariantMap();

	QHash<QString, RestService*> servicesHash;
	for (const auto& service: servicesMap.keys())
	{
		const auto url = servicesMap.value(service).toString();
		servicesHash.insert(service, new RestService(service, url, parent()));
	}

	Q_EMIT services(servicesHash);
}

}


