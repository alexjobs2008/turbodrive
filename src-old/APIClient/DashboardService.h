#ifndef DASHBOARD_SERVICE_H
#define DASHBOARD_SERVICE_H

#include "Network/RestResource.h"

#define DASHBOARD_SERVICE_NAME "DashboardService"

namespace Drive
{

class RegisterLinkResource;
typedef QSharedPointer<RegisterLinkResource> RegisterLinkResourceRef;

class RegisterLinkResource : public RestResource
{
    Q_OBJECT
public:

    static RegisterLinkResourceRef create();

    void requestRegisterLink();
    
    virtual QString path() const;
    virtual QString service() const;
    virtual bool restricted() const;

signals:
    void linkReceived(const QString& link);

private:
    virtual bool processGetResponse(int status, const QByteArray& data,
        const HeaderList& headers);
};

class PasswordResetResource;
typedef QSharedPointer<PasswordResetResource> PasswordResetResourceRef;

class PasswordResetResource : public RestResource
{
    Q_OBJECT
public:

    static PasswordResetResourceRef create();

    void resetPassword(const QString& email);

    virtual QString path() const;
    virtual QString service() const;
    virtual bool restricted() const;

signals:
    void resetSuccessfully();
    void resetFailed(const QString& error);

private:
    virtual bool processPostResponse(int status, const QByteArray& data,
        const HeaderList& headers);

    QByteArray PasswordResetResource::toByteArray(const QString& email);
};

}

#endif // DASHBOARD_SERVICE_H