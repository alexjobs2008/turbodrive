#ifndef APP_STRINGS_H
#define APP_STRINGS_H

#include <QtCore/QObject>

namespace Strings
{
    const QString companyNameEn = "Assistent";
    const QString companyName = QObject::tr("Assistent");
    const QString companyDomain = "assistent.by";
    const QString appNameEn = "Drive";
    const QString appName = QObject::tr("Drive");
    const QString appVersion = "1.0a";
    const QString websiteText = QObject::tr("www.assistent.by");
    const QString websiteLink = "http://assistent.by";
    const QString copyright = QObject::tr(
        "Copyright (c) 2013 Assistent, Inc.\nAll rights reserved.");


}

#endif // APP_STRINGS_H