#include "RestDispatcher.h"
#include "QsLog/QsLog.h"

RestDispatcher::RestDispatcher(QObject *parent)
    : QObject(parent)
{
}


void RestDispatcher::request(const RestResource::RequestRef &restResourceRequest)
{
    QLOG_TRACE() << restResourceRequest->_path;
    /* If we're Authenticated then all the request should go into the Authenticated queue
       If we're Unauthenticated then Restricted requests should go into the Authenticated queue while
                                      Unrestricted requests should go into the Unauthenticated queue
     */
    Q_ASSERT(_services.contains(restResourceRequest->_service));
    RestService* service = _services.value(restResourceRequest->_service);
    
    switch(_mode)
    {
    case Authorized:
        service->_restResourceRequestsAuthenticated.enqueue(restResourceRequest);
        break;
    case Unauthorized:
        if (restResourceRequest->_restResource->restricted())
            service->_restResourceRequestsAuthenticated.enqueue(restResourceRequest);
        else
            service->_restResourceRequestsUnauthenticated.enqueue(restResourceRequest);
        break;
    default:
        W_ASSERT(false);
        break;
    }
    next();
}