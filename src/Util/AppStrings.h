#ifndef APP_STRINGS_H
#define APP_STRINGS_H

#include <QtCore/QString>

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

static const QString s_company = QString::fromLatin1("MTS");
static const QString s_domain = QString::fromLatin1("http://disk.mts.by/");
static const QString s_application = QString::fromLatin1("Disk");
static const QString s_version = QString::fromLatin1(VERSION);

}

}

#endif // APP_STRINGS_H
