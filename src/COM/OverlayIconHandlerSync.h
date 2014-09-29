#ifndef OVERLAYICONHANDLERSYNC_H
#define OVERLAYICONHANDLERSYNC_H

#include <QObject>
#include <ActiveQt/QAxBindable>

namespace Drive
{


class OverlayIconHandlerSync : public QObject, public QAxBindable
{
    Q_OBJECT

    Q_CLASSINFO("ClassID", "{394f2bb6-af35-4c93-b5dd-b75fa44b1526}")
    Q_CLASSINFO("InterfaceID", "{a9507627-582b-41cf-bfa9-88039c795ead}")
    Q_CLASSINFO("RegisterObject", "yes")

public:
    OverlayIconHandlerSync(QObject *parent = 0);

    // QAxBindable interface
public:
    QAxAggregated *createAggregate();
};


} // namespace Drive


#endif // OVERLAYICONHANDLERSYNC_H
