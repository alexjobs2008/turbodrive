#include "AppStrings.h"

namespace Drive
{

namespace Strings
{

const QString getAppString(AppStringType type)
{
    return QObject::tr(appStrings[type]);
}

}

}