#include "connection_controller.h"

ConnectionController::ConnectionController(QObject *parent)
    : QObject(parent)
    , state(Disconnected)
{
}