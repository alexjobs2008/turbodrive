#ifndef APP_STRINGS_H
#define APP_STRINGS_H

#include <QtCore/QObject>

namespace Drive
{

namespace Strings
{

enum AppStringType
{
	CompanyName = 0,
	AppName,
	AppFullName,
	WebSiteText,
	Copyright
};

const QString getAppString(AppStringType type);

static const char* s_company = "MTS";
static const char* s_domain = "http://disk.mts.by/";
static const char* s_application = "Disk";
static const char* s_version = "1.0";
static const char* s_url = "http://disk.mts.by/";

}

}

#endif // APP_STRINGS_H
