#include "OverlayIconHandlerSync.h"

#include "QsLog.h"
#include "singleapp.h"
#include "overlayiconshandler.h"
#include "FileUtils.h"

#include <ActiveQt/QAxFactory>

namespace Drive
{


OverlayIconHandlerSync::OverlayIconHandlerSync(QObject *parent) :
    QObject(parent)
{
    setObjectName("MTS Drive::OverlayIconHandlerSync");
}

QAxAggregated *OverlayIconHandlerSync::createAggregate()
{
    return new OverlayIconsHandler(FOLDER_ICON_SYNC - 1);
}


} // namespace Drive
