#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore/QSettings>

class Settings : public QObject
{
    Q_OBJECT
private:
    Settings(QObject *parent = 0);
    ~Settings();
    Q_DISABLE_COPY(Settings);

public:
    static Settings& instance();

    QString folder() const;
    void setFolder(const QString& folderPath);
    
    bool autostart() const;
    void setAutostart(bool isOn);

    bool desktopNotifications() const;
    void setDesktopNotifications(bool isOn);

public slots:

#ifndef Q_OS_MACX
    void apply();
#endif

signals:
    void autostartChanged(bool isOn);
    void folderChanged(const QString& folderPath);
    void desktopNotificationsChanged(bool isOn);

    void gotDirty();

private:
    QString defaultFolderPath() const;

    QSettings *settings;
};

#ifdef Q_OS_WIN

class WindowsAutoexec
{
public:
    static bool get();
    static void set(bool autoexec);
};

#endif // Q_OS_WIN

#endif // SETTINGS_H