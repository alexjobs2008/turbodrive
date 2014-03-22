#ifndef REST_DISPATCHER_H
#define REST_DISPATCHER_H

#include "Network/GeneralRestDispatcher.h"

namespace Drive
{

class RestDispatcher : public GeneralRestDispatcher
{
    Q_OBJECT
public:

    static RestDispatcher& instance();


protected slots:
    virtual void loadServices();

private:
    RestDispatcher(QObject *parent = 0);
    ~RestDispatcher();
    Q_DISABLE_COPY(RestDispatcher);



};

}

#endif // REST_DISPATCHER_H