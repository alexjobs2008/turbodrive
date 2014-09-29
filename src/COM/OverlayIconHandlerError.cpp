#include "OverlayIconHandlerError.h"

#include "QsLog.h"
#include "singleapp.h"
#include "overlayiconshandler.h"
#include "FileUtils.h"

#include <ActiveQt/QAxFactory>

namespace Drive
{


OverlayIconHandlerError::OverlayIconHandlerError(QObject *parent) :
    QObject(parent)
{
    setObjectName("MTS OverlayIconHandlerOk");
}

QAxAggregated *OverlayIconHandlerError::createAggregate()
{
    return new OverlayIconsHandler(FOLDER_ICON_ERROR - 1);
}


} // namespace Drive
