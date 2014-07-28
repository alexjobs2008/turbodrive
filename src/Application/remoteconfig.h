#ifndef REMOTE_CONFIG_H
#define REMOTE_CONFIG_H

#include "Network/SimpleDownloader.h"

#include <QtCore>
#include <memory>

class RestService;

namespace Drive
{

class RemoteConfig: public QObject
{
	Q_OBJECT

public:
	RemoteConfig(const QString& url, QObject* parent = nullptr);

	Q_SIGNAL void services(const QHash<QString, RestService*>&);
	Q_SIGNAL void update(const QString& version);

	QString updateUrl() const;

private:
	void start();

	void onDownloadFinished(const QByteArray& data);

	void parseServices(const QJsonDocument& doc);
	void parseUpdate(const QJsonDocument& doc);

private:
	std::unique_ptr<SimpleDownloader> m_downloader;
	QTimer m_timer;
	QString m_configUrl;
	QString m_updateUrl;
};

}

#endif // REMOTE_CONFIG_H
