#include <ActiveQt/QAxFactory>

#include "OverlayIconHandlerSync.h"
#include "OverlayIconHandlerOk.h"
#include "OverlayIconHandlerError.h"

QAXFACTORY_BEGIN("{c67ef0c5-8f50-4774-8602-43583de70f68}", "{b34964de-dfcb-4dae-b69a-bf02ebd3a266}")
    QAXCLASS(Drive::OverlayIconHandlerSync)
    QAXCLASS(Drive::OverlayIconHandlerOk)
    QAXCLASS(Drive::OverlayIconHandlerError)
QAXFACTORY_END()

