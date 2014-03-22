#ifndef FILES_SERVICE_H
#define FILES_SERVICE_H

#include "Network/RestResource.h"

#include <QtCore/QVariantMap>

class QJsonObject;

namespace Drive
{

class OnlineRestResource;
typedef QSharedPointer<OnlineRestResource> OnlineRestResourceRef;

class OnlineRestResource : public RestResource
{
    Q_OBJECT
public:    

    static OnlineRestResourceRef create();

    void ping();

    virtual QString path() const;
    virtual QString service() const;
    virtual bool restricted() const;

signals:
    void pingOk();
    void pingError();

private:
    virtual bool processGetResponse(int status, const QByteArray& data,
        const HeaderList& headers);
};

}

#endif // FILES_SERVICE_H