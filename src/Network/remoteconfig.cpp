#include "remoteconfig.h"

#include "SimpleDownloader.h"
#include "RestService.h"

#include <QsLog/QsLog.h>
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

//{
//	data: {
//		update: {
//			version: "1.0.0.0",
//			url: "http://disk.mts.by/applications/MTS_Drive.exe"
//		}
//		environment: {
//			name: "MTS",
//			services: {
//				"AuthService": "http://disk.mts.by",
//				"DashboardService": "http://disk.mts.by",
//				"ProfileService": "http://disk.mts.by",
//				"FilesService": "http://disk.mts.by",
//				"ContentService": "http://disk.mts.by",
//				"SharingService": "http://disk.mts.by",
//				"NotificationService": "http://rpl.disk.mts.by"
//			}
//		}
//	}
//}

void RemoteConfig::onDownloadFinished(const QByteArray&)
{
	QJsonObject json_services;
	json_services.insert(QLatin1String("AuthService"), QLatin1String("http://disk.mts.by"));
	json_services.insert(QLatin1String("DashboardService"), QLatin1String("http://disk.mts.by"));
	json_services.insert(QLatin1String("ProfileService"), QLatin1String("http://disk.mts.by"));
	json_services.insert(QLatin1String("FilesService"), QLatin1String("http://disk.mts.by"));
	json_services.insert(QLatin1String("ContentService"), QLatin1String("http://disk.mts.by"));
	json_services.insert(QLatin1String("SharingService"), QLatin1String("http://disk.mts.by"));
	json_services.insert(QLatin1String("NotificationService"), QLatin1String("http://rpl.disk.mts.by"));

	QJsonObject json_environment;
	json_environment.insert(QLatin1String("name"), QLatin1String("MTS"));
	json_environment.insert(QLatin1String("services"), json_services);

	QJsonObject json_update;
	json_update.insert(QLatin1String("version"), QLatin1String("1.0.0.0"));
	json_update.insert(QLatin1String("url"), QLatin1String("http://disk.mts.by/applications/MTS_Drive.exe"));

	QJsonObject json_data;
	json_data.insert(QLatin1String("update"), json_update);
	json_data.insert(QLatin1String("environment"), json_environment);

	QJsonObject json_main;
	json_main.insert(QLatin1String("data"), json_data);

	const QByteArray d =
			QJsonDocument(json_main).toJson(QJsonDocument::Compact);
	QLOG_DEBUG() << d;










	QJsonParseError error;

	const QByteArray data = d;
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


