#include "ConnectionController.h"

ConnectionController::ConnectionController(QObject *parent)
	: QObject(parent)
	, state(Disconnected)
{
}