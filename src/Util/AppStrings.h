#ifndef APP_STRINGS_H
#define APP_STRINGS_H

#include <QtCore/QObject>

#define STRINGS_CONTEXT "AppStrings"

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

static const char* appStrings[] =
{
    QT_TRANSLATE_NOOP(STRINGS_CONTEXT, "Assistent"),
    QT_TRANSLATE_NOOP(STRINGS_CONTEXT, "Disk"),
    QT_TRANSLATE_NOOP(STRINGS_CONTEXT, "Assistent.Disk"),
    QT_TRANSLATE_NOOP(STRINGS_CONTEXT, "www.assistent.by"),
    QT_TRANSLATE_NOOP(STRINGS_CONTEXT, "Copyright (c) 2013 Assistent, Inc.\n"
                                       "All rights reserved.")
};

const QString getAppString(AppStringType type);

static const char* companyNameEn = "Assistent";
static const char* companyDomain = "assistent.by";
static const char* appNameEn = "Drive";
static const char* appFullNameEn = "Assistent.Drive";
static const char* appVersion = "1.0b";
static const char* websiteLink = "http://assistent.by";

}

}

#endif // APP_STRINGS_H