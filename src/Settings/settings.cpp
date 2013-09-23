#include "settings.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>




Settings::Settings(QObject *parent)
    : QObject(parent)
{
    read();
}

Settings::~Settings()
{
}

Settings& Settings::instance()
{
    static Settings settings;
    return settings;
}

void Settings::read()
{

}

void Settings::write()
{

}



void WindowsAutoexec::set(bool autoexec)
{
    QSettings settings(QSettings::NativeFormat, QSettings::UserScope,
        "Microsoft", "Windows");

    settings.beginGroup("CurrentVersion");
    settings.beginGroup("run");
    if (autoexec)
    {
        settings.setValue(QCoreApplication::applicationName(),
            QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
    }
    else
    {
        settings.remove(QCoreApplication::applicationName());
    }
    settings.endGroup();
    settings.endGroup();    
}