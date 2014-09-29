#include "OverlayIconHandlerOk.h"

#include "QsLog.h"
#include "singleapp.h"
#include "overlayiconshandler.h"
#include "FileUtils.h"

#include <ActiveQt/QAxFactory>

namespace Drive
{


OverlayIconHandlerOk::OverlayIconHandlerOk(QObject *parent) :
    QObject(parent)
{
    setObjectName("MTS Drive::OverlayIconHandlerOk");
}

QAxAggregated *OverlayIconHandlerOk::createAggregate()
{
    return new OverlayIconsHandler(FOLDER_ICON_OK - 1);
}


} // namespace Drive
