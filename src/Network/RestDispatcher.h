#ifndef REST_DISPATCHER_H
#define REST_DISPATCHER_H

#include <QtCore/QObject>

class RestDispatcher : public QObject
{
    Q_OBJECT
public:
    static RestDispatcher& instance();

    void request(const RestResource::RequestRef& request);
    void cancelAll(const RestResourceRef& resource);
    void cancelAll();

    //RestDispatcher(QObject *parent = 0);

signals:

public slots:

private:

};

#endif // REST_DISPATCHER_H