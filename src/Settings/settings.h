#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore/QSettings>

static const QString localizedOrganizationName("Assistent");
static const QString localizedApplicationName("Drive");

class Settings : public QObject
{
    Q_OBJECT
private:
    Settings(QObject *parent = 0);
    ~Settings();
    Q_DISABLE_COPY(Settings);

public:
    static Settings& instance();

public slots:
    void read();
    void write();

private:

};

class WindowsAutoexec
{
public:
    static bool get();
    static void set(bool autoexec);
};

#endif SETTINGS_H