#include "proxySettings.h"
#include "QsLog//QsLog.h"

QDataStream &operator<<(QDataStream& out, const ProxyUsage& proxyUsage)
{
    QLOG_DEBUG() << "<<" << proxyUsage;
    out << (quint32&)proxyUsage;
    return out;
}

QDataStream &operator>>(QDataStream& in, ProxyUsage& proxyUsage)
{
    QLOG_DEBUG() << ">>";
    in >> (quint32&)proxyUsage;
    return in;
}

QDataStream &operator<<(QDataStream& out, const ProxySettings& proxySettings)
{
    out << (quint32&)proxySettings.kind
        << proxySettings.server
        << proxySettings.port
        << (quint32&)proxySettings.loginRequired
        << proxySettings.username
        << proxySettings.password;
    
    return out;
}

QDataStream &operator>>(QDataStream& in, ProxySettings& proxySettings)
{
    in >> (quint32&)proxySettings.kind
       >> proxySettings.server
       >> proxySettings.port
       >> (quint32&)proxySettings.loginRequired
       >> proxySettings.username
       >> proxySettings.password;

    return in;
}