#ifndef REMOTE_CONFIG_H
#define REMOTE_CONFIG_H

#include <QtCore>

class RestService;
class SimpleDownloader;

namespace Drive
{

class RemoteConfig: public QObject
{
	Q_OBJECT
public:
	RemoteConfig(const QString& url, QObject* parent);

	Q_SIGNAL void services(const QHash<QString, RestService*>&);

private:
	void onDownloadFinished(const QByteArray& data);

private:
	SimpleDownloader* m_downloader;
};

}

#endif // REMOTE_CONFIG_H
