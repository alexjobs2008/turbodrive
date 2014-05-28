#include "remoteconfig.h"

#include "Network/SimpleDownloader.h"
#include "Network/RestService.h"

#include <QtCore>

namespace Drive
{

RemoteConfig::RemoteConfig(const QString& url, QObject* parent)
	: QObject(parent)
	, m_downloader(new SimpleDownloader(url, SimpleDownloader::Data, this))
{
	connect(m_downloader.get(), &SimpleDownloader::finished,
			this, &RemoteConfig::onDownloadFinished);
}

QString RemoteConfig::updateUrl() const
{
	return m_updateUrl;
}

void RemoteConfig::onDownloadFinished(const QByteArray& data)
{
	m_downloader.reset();

	QJsonParseError error;

	auto doc = QJsonDocument::fromJson(data, &error);
	Q_ASSERT(error.error == QJsonParseError::NoError);

	parseServices(doc);
	parseUpdate(doc);
}

void RemoteConfig::parseServices(const QJsonDocument& doc)
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

void RemoteConfig::parseUpdate(const QJsonDocument& doc)
{
	static const auto dataKey = QString::fromLatin1("data");
	static const auto updateKey = QString::fromLatin1("update");
	static const auto versionKey = QString::fromLatin1("version");
	static const auto urlKey = QString::fromLatin1("url");

	const QJsonObject updateObj = doc.object()
			.value(dataKey).toObject()
			.value(updateKey).toObject();

	const QString version = updateObj.value(versionKey).toString();
	m_updateUrl = updateObj.value(urlKey).toString();

	if (!version.isEmpty() && version != QCoreApplication::applicationVersion())
	{
		Q_EMIT update(version);
	}
}

}


