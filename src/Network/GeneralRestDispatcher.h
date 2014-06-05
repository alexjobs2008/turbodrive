#ifndef GENERAL_REST_DISPATCHER_H
#define GENERAL_REST_DISPATCHER_H

#include "RestResource.h"

#include <QtCore/QObject>
#include <QtCore/QDateTime>

class RestNetworkAccessManager;
class RestService;
class QUrl;
class QNetworkReply;

class GeneralRestDispatcher : public QObject
{
    Q_OBJECT
public:

    enum Mode
    {
        Unauthorized,
        Authorized
    };

    enum ReconnectErrorReason
    {
        InvalidCredentials = 0,
        OperationJustFailed
    };

    explicit GeneralRestDispatcher(QObject *parent = 0);

    //static GeneralRestDispatcher& instance();

    void request(const RestResource::RequestRef& request);
    void cancelAll();
    void cancelAll(const RestResourceRef& resource);    

    QUrl buildUrl(const QString& serviceName, const QString &path) const;
    QUrl buildUrl(const RestResource* restResource);

signals:
    void dispatcherAboutToBeAuthorized();
    void dispatcherAuthorized();
    void dispatcherUnauthorized();
    void dispatcherReconnectFailed(GeneralRestDispatcher::ReconnectErrorReason);
    void proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*);

protected slots:
    virtual void loadServices() = 0;

private slots:    
    void replyFinished(QNetworkReply* networkReply);

private:
    void next();
    void setMode(Mode newMode);

    void cancelAll(QQueue<RestResource::RequestRef>& queue,
        const RestResourceRef* restResource = 0);
    
    void cancelCurrent(RestService* service,
        const RestResourceRef* restResource = 0);

    QNetworkRequest createRequest(RestService* service) const;
    void doOperation(RestResource::Operation operation, RestService* service);

    Mode mode;
    RestNetworkAccessManager *networkAccessManager;
    QHash<QString, RestService*> services;
    QString authToken;
    QDateTime authTimestamp;

};

#endif // GENERAL_REST_DISPATCHER_H