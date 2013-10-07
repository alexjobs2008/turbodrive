#ifndef REST_SERVICE_H
#define REST_SERVICE_H

#include "RestResource.h"

#include <QtCore/QObject>
#include <QtCore/QQueue>

class RestDispatcher;

class RestService : public QObject
{
    Q_OBJECT
public:
    explicit RestService(const QString& name, const QString& address,
        QObject* parent = 0);
    
    virtual ~RestService();

    inline bool queuesAreEmpty()
    {
        return authenticatedRequests.isEmpty()
            && unauthenticatedRequests.isEmpty();
    }
    
    const QString& name() const;
    const QString& address() const;

private:
    const QString serviceName;
    const QString serviceAddress;
    RestResource::RequestRef currentRequest;
    QQueue<RestResource::RequestRef> authenticatedRequests;
    QQueue<RestResource::RequestRef> unauthenticatedRequests;

    friend class RestDispatcher;
};

#endif // REST_SERVICE_H