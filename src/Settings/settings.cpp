#include "settings.h"
#include "proxySettings.h"
#include "Util/AppStrings.h"
#include "QsLog/QsLog.h"

#include <QtCore/QMetaType>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QLocale>

namespace Drive
{

const QString Settings::email("email");
const QString Settings::password("password");
const QString Settings::autoLogin("autoLogin");
const QString Settings::folderPath("folderPath");
const QString Settings::desktopNotifications("desktopNotifications");
const QString Settings::autostart("autostart");
const QString Settings::language("language");
const QString Settings::limitDownload("limitDownload");
const QString Settings::limitUpload("limitUpload");
const QString Settings::downloadSpeed("downloadSpeed");
const QString Settings::uploadSpeed("uploadSpeed");
const QString Settings::proxyUsage("proxyUsage");
const QString Settings::proxyCustomSettings("proxySettings");
const QString Settings::env("env");

#define DEFAULT_DOWNLOAD_SPEED 50
#define DEFAULT_UPLOAD_SPEED 50

Settings::Settings(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<ProxyUsage>("ProxyUsage");
    qRegisterMetaTypeStreamOperators<ProxyUsage>("ProxyUsage");
    qRegisterMetaType<ProxySettings>("ProxySettings");
    qRegisterMetaTypeStreamOperators<ProxySettings>("ProxySettings");

    settings = new QSettings(this);

#ifdef Q_OS_MAC
    applyImmediately = true
#else
    applyImmediately = false;
#endif
}

Settings::~Settings()
{
}

Settings& Settings::instance()
{
    static Settings settings;
    return settings;
}

QVariant Settings::get(const QString& settingName) const
{
    if (settingName == autostart)
    {
#ifdef Q_OS_WIN
        return WindowsAutoexec::get();
#endif

#ifdef Q_OS_MACX
        return false;
#endif
    }
    
//    QLOG_DEBUG() << "getting setting value: " << settingName;
    if (settingName == proxyUsage)
    {
//        QLOG_DEBUG() << "obtaining proxyUsage...";
        if (settings->contains(settingName))
        {
//            QLOG_DEBUG() << "we have it: " << settings->value(settingName).toInt();
//            QLOG_DEBUG() << "returning: " << settings->value(settingName);
            return settings->value(settingName);
        }
//        else
//        {
//            QLOG_DEBUG() << "we DON'T have it";
//        }
    }

    return settings->value(settingName, defaultSettingValue(settingName));
}

void Settings::set(const QString& settingName, QVariant value, Kind kind)
{
    if (applyImmediately || kind == RealSetting)
    {
        if (settingName == autostart)
        {
#ifdef Q_OS_WIN
            if (WindowsAutoexec::get() != value.toBool())
                WindowsAutoexec::set(value.toBool());
            
            return;
#else
            return;
#endif
        }
        
        QVariant oldValue;

        QLOG_TRACE() << "Setting" << settingName << "change:" << value;

        if (settings->contains(settingName))
        {
            oldValue = settings->value(settingName);
            QLOG_TRACE() << "Old" << settingName << "value was:" << oldValue;
            if (oldValue == value)
            {
                QLOG_TRACE() <<
                    "Not applying the settings, since the value hasn't changed";
                return;
            }
        }

        settings->setValue(settingName, value);
        emit settingChanged(settingName, oldValue, value);
    }
    else
    {
        QLOG_TRACE() << "Candidate setting" <<
            settingName << "change:" << value;

        candidateSettings.insert(settingName, value);
        emit dirtyStateChanged(true);
    }
}

QList<int> Settings::supportedLanguages()
{
    QList<int> langs; // TODO: make all this static
    langs << QLocale::English << QLocale::Russian;
    return langs;
}

void Settings::apply()
{
    if (applyImmediately)
        return;

    if (!candidateSettings.size())
        return;

    QMapIterator<QString, QVariant> i(candidateSettings);
    while (i.hasNext()) {
        i.next();
        set(i.key(), i.value(), RealSetting);        
    }

    candidateSettings.clear();
    emit dirtyStateChanged(false);

    QString folderPath =
        Settings::instance().get(Settings::folderPath).toString();

    QDir dir;
    dir.mkpath(folderPath);
}

void Settings::cancel()
{
    if (applyImmediately)
        return;

    candidateSettings.clear();
    emit dirtyStateChanged(false);
}

QVariant Settings::defaultSettingValue(const QString& settingName) const
{
    if (settingName == folderPath)
    {
        QString homePath = QStandardPaths::
            standardLocations(QStandardPaths::HomeLocation).at(0);

        return QString("%1/%2").arg(homePath).arg(Strings::appNameEn);
    }

    if (settingName == desktopNotifications)
        return true;

    if (settingName == email)
        return QString();

    if (settingName == password)
        return QString();

    if (settingName == autoLogin)
        return true;    

    if (settingName == language)
        return QLocale::English;

    if (settingName == limitDownload)
        return false;

    if (settingName == limitUpload)
        return false;
    
    if (settingName == downloadSpeed)
        return DEFAULT_DOWNLOAD_SPEED;

    if (settingName == uploadSpeed)
        return DEFAULT_UPLOAD_SPEED;

    if (settingName == proxyUsage)
        return ProxyUsage::NoProxy;   


    if (settingName == proxyCustomSettings)
    {
        ProxySettings proxySettings;
        proxySettings.kind = ProxySettings::HttpServer;
        proxySettings.server = QString();
        proxySettings.port = QString();
        proxySettings.loginRequired = false;
        proxySettings.username = QString();
        proxySettings.password = QString();
        return QVariant::fromValue(proxySettings);
    }

    if (settingName == env)
        //return AssistentDotThVersionX;
        //return Env::AssistentDotTh;
        return Env::NewAssistentDotBy;

    QLOG_DEBUG() << "Setting" << settingName << "has no default value.";
    return QVariant();
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

}
