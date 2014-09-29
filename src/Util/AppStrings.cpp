#include "AppStrings.h"

#include <QtCore/QObject>

namespace Drive
{

namespace Strings
{

const QString getAppString(AppStringType type)
{
	static const char* stringsContext = "AppStrings";
	static const QString s_appStrings[] =
	{
		QObject::tr("MTS", stringsContext),
		QObject::tr("Disk", stringsContext),
		QObject::tr("MTS Disk", stringsContext),
		QObject::tr("http://disk.mts.by/", stringsContext),
        QObject::tr("https://play.google.com/store/apps/details?id=by.mts.mtsdrive", stringsContext),
        QObject::tr("https://itunes.apple.com/by/app/mts-disk/id869283906?mt=8", stringsContext),
        QObject::tr("Copyrights (c) MTS\nAll rights reserved.", stringsContext)
	};
	return s_appStrings[type];
}

}

}
