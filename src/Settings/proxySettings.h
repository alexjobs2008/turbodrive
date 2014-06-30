#ifndef PROXY_SETTINGS_H
#define PROXY_SETTINGS_H

#include <QtCore/QMetaType>
#include <QtCore/QDataStream>

namespace Drive
{

enum ProxyUsage
{
	NoProxy = 0,
	AutodetectProxy,
	CustomProxy
};

struct ProxySettings
{
	enum Kind
	{
		HttpServer = 0,
		SocksServer
	};

	Kind kind;
	QString server;
	QString port;
	bool loginRequired;
	QString username;
	QString password;
};

}

Q_DECLARE_METATYPE(Drive::ProxyUsage)
Q_DECLARE_METATYPE(Drive::ProxySettings)

QDataStream &operator<<(QDataStream& out, const Drive::ProxyUsage& proxyUsage);
QDataStream &operator>>(QDataStream& in, Drive::ProxyUsage& proxyUsage);

QDataStream &operator<<(QDataStream& out, const Drive::ProxySettings& proxySettings);
QDataStream &operator>>(QDataStream& in, Drive::ProxySettings& proxySettings);

#endif // PROXY_SETTINGS
