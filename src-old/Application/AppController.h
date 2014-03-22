#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "types.h"
#include "APIClient/ApiTypes.h"

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSystemTrayIcon>
#include <QtCore/QSharedPointer>

class QMenu;
class QAction;

namespace Drive
{

class SettingsWidget;

class AppController : public QMainWindow
{
    Q_OBJECT

public:
    static AppController& instance();
    
    State state() const;
    
    QString authToken() const;
    void setAuthToken(const QString &token);

    ProfileData profileData() const;
    void setProfileData(const ProfileData& data);
    
public slots:
    void setState(State newState);

signals:
    void stateChanged(Drive::State state);
    void profileDataUpdated(const ProfileData& data);

private slots:
    void on_actionOpenFolder_triggered();
    void on_actionPause_triggered();
    void on_actionResume_triggered();
    void on_actionPreferences_triggered();
    void on_actionExit_triggered();

    void on_trayIcon_activated(QSystemTrayIcon::ActivationReason reason);
    void on_settingsWidget_logout();

    void onLoginFinished();

private:
    Q_DISABLE_COPY(AppController)
    explicit AppController(QWidget *parent = 0);

    void createActions();
    void createTrayIcon();
    void createSettingsWidget();

    QSharedPointer<SettingsWidget> settingsWidget;    

    QMenu *trayMenu;
    
    QAction *actionOpenFolder;
    QAction *actionPause;
    QAction *actionResume;
    QAction *actionPreferences;
    QAction *actionExit;

    State currentState;

    ProfileData currentProfileData;
    QString currentAuthToken;
};

}

#endif // MAINWINDOW_H
