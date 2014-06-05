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

	parseServices(data);
	parseUpdate(data);
}

void RemoteConfig::parseServices(const QByteArray& data)
{
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

void RemoteConfig::parseUpdate(const QByteArray& data)
{
	static const auto dataKey = QString::fromLatin1("data");
	static const auto updateKey = QString::fromLatin1("update");
	static const auto versionKey = QString::fromLatin1("version");
	static const auto urlKey = QString::fromLatin1("url");

	const QJsonObject update = doc.object()
			.value(dataKey).toObject()
			.value(updateKey).toObject();

	const QString version = update.value(versionKey).toString();
	const QString url = update.value(urlKey).toString();

	if (version != QCoreApplication::applicationVersion())
	{
		Q_EMIT update(version, url);
	}
}

}


