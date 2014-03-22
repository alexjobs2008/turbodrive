#include "RestDispatcher.h"


namespace Drive
{

RestDispatcher& RestDispatcher::instance()
{
    static RestDispatcher restDispatcher;
    return restDispatcher;
}

RestDispatcher::RestDispatcher(QObject *parent)
    : GeneralRestDispatcher(parent)
{
}

void RestDispatcher::loadServices()
{

}

}