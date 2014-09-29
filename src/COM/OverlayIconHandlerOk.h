#ifndef OVERLAYICONHANDLEROK_H
#define OVERLAYICONHANDLEROK_H

#include <QObject>
#include <ActiveQt/QAxBindable>

namespace Drive
{


class OverlayIconHandlerOk : public QObject, public QAxBindable
{
    Q_OBJECT

    Q_CLASSINFO("ClassID", "{b50a71db-c4a7-4551-8d14-49983566afee}")
    Q_CLASSINFO("InterfaceID", "{4a427759-16ef-4ed8-be79-59ffe5789042}")
    Q_CLASSINFO("RegisterObject", "yes")

public:
    OverlayIconHandlerOk(QObject *parent = 0);

    // QAxBindable interface
public:
    QAxAggregated *createAggregate();
};


} // namespace Drive

#endif // OVERLAYICONHANDLEROK_H
