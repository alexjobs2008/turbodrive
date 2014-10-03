#include <ActiveQt/QAxFactory>
#include <QApplication>

#include "OverlayIconHandlerSync.h"
#include "OverlayIconHandlerOk.h"
#include "OverlayIconHandlerError.h"

QAXFACTORY_BEGIN("{c67ef0c5-8f50-4774-8602-43583de70f68}", "{b34964de-dfcb-4dae-b69a-bf02ebd3a266}")
    QAXCLASS(Drive::OverlayIconHandlerSync)
    QAXCLASS(Drive::OverlayIconHandlerOk)
    QAXCLASS(Drive::OverlayIconHandlerError)
QAXFACTORY_END()

/*
int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    // started by COM - don't do anything
    if (QAxFactory::isServer())
        return app.exec();

    // started by user
    // Application appobject(0);
    // appobject.setObjectName("From Application");

    Drive::OverlayIconHandlerSync appobject;

    bool started = QAxFactory::startServer();
    (void)started;
    QAxFactory::registerActiveObject(&appobject);

    // appobject.setVisible(true);

    // QObject::connect(qApp, SIGNAL(lastWindowClosed()), &appobject, SLOT(quit()));

    return app.exec();
}
*/
