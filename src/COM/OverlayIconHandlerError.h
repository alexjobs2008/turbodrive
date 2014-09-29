#ifndef OVERLAYICONHANDLERERROR_H
#define OVERLAYICONHANDLERERROR_H

#include <QObject>
#include <ActiveQt/QAxBindable>

namespace Drive
{


class OverlayIconHandlerError : public QObject, public QAxBindable
{
    Q_OBJECT

    Q_CLASSINFO("ClassID", "{76aa4b36-471f-4de8-8f4d-ddf1f8a77909}")
    Q_CLASSINFO("InterfaceID", "{100103ff-974d-4c90-bbed-5342caec963f}")
    Q_CLASSINFO("RegisterObject", "yes")

public:
    OverlayIconHandlerError(QObject *parent = 0);

    // QAxBindable interface
public:
    QAxAggregated *createAggregate();
};


} // namespace Drive

#endif // OVERLAYICONHANDLERERROR_H
