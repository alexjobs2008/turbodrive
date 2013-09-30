#include "settings.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

static const QString sFolder("folder");

static const QString sEmail("email");
static const QString sPassword("password");

static const QString sAutostart("autostart");
static const QString sDesktopNotifications("desktopNotifications");

Settings::Settings(QObject *parent)
    : QObject(parent)
{
    settings = new QSettings(this);
}

Settings::~Settings()
{
}

Settings& Settings::instance()
{
    static Settings settings;
    return settings;
}

QString Settings::folder() const
{
    return settings->value(sFolder, defaultFolderPath()).toString();
}

void Settings::setFolder(const QString& folderPath)
{
    if (settings->contains(sFolder)
        && settings->value(sFolder).toString() == folderPath)
        return;

    settings->setValue(sFolder, folderPath);
    emit folderChanged(folderPath);
}

bool Settings::autostart() const
{
#ifdef Q_OS_WIN
    return WindowsAutoexec::get();
#endif

#ifdef Q_OS_MACX
    return false;
#endif
}

void Settings::setAutostart(bool isOn)
{
#ifdef Q_OS_WIN
    if (autostart() == isOn)
        return;

    WindowsAutoexec::set(isOn);
    emit autostartChanged(isOn);        
#endif

#ifdef Q_OS_MACX
    return false;
#endif    
}

bool Settings::desktopNotifications() const
{
    return settings->value(sDesktopNotifications, true).toBool();
}

void Settings::setDesktopNotifications(bool isOn)
{
    if (settings->contains(sDesktopNotifications)
        && settings->value(sDesktopNotifications).toBool() == isOn)
        return;

    settings->setValue(sDesktopNotifications, isOn);
    emit desktopNotificationsChanged(isOn);
}

#ifndef Q_OS_MACX

void Settings::apply()
{

}

#endif

QString Settings::defaultFolderPath() const
{
    QString homePath =
        QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0);

    return QString("%1/%2").arg(homePath).arg("Drive");
}


#ifdef Q_OS_WIN

bool WindowsAutoexec::get()
{    
    QSettings settings(QSettings::NativeFormat, QSettings::UserScope,
        "Microsoft", "Windows");

    settings.beginGroup("CurrentVersion");
    settings.beginGroup("run");
    QString path = settings.value(QCoreApplication::applicationName(),
        QString()).toString();

    settings.endGroup();
    settings.endGroup();

    return path ==
        QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
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

#endif // Q_OS_WIN