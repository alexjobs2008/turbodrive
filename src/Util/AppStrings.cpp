#include "AppStrings.h"

#define STRINGS_CONTEXT "AppStrings"

namespace Drive
{

namespace Strings
{

const QString getAppString(AppStringType type)
{
	static QString s_appStrings[] =
	{
		QObject::tr("MTS", STRINGS_CONTEXT),
		QObject::tr("Disk", STRINGS_CONTEXT),
		QObject::tr("MTS Disk", STRINGS_CONTEXT),
		QObject::tr("http://disk.mts.by/", STRINGS_CONTEXT),
		QObject::tr("Copyrights (c) MTS\nAll rights reserved.", STRINGS_CONTEXT)
	};
	return s_appStrings[type];
}

}

}
