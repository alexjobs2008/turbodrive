#ifndef PROXY_SETTINGS_H
#define PROXY_SETTINGS_H

#include <QtCore\QMetaType>
#include <QtCore\QDataStream>

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

Q_DECLARE_METATYPE(ProxyUsage)
Q_DECLARE_METATYPE(ProxySettings)

QDataStream &operator<<(QDataStream& out, const ProxyUsage& proxyUsage);
QDataStream &operator>>(QDataStream& in, ProxyUsage& proxyUsage);

QDataStream &operator<<(QDataStream& out, const ProxySettings& proxySettings);
QDataStream &operator>>(QDataStream& in, ProxySettings& proxySettings);

#endif // PROXY_SETTINGS