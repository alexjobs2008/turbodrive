#include "AppController.h"
#include "LoginController.h"
#include "TrayIcon.h"

#include "Util/AppStrings.h"
#include "Util/FileUtils.h"

#include "Settings/settings.h"
#include "SettingsUI/SettingsWidget.h"

#include "QsLog/QsLog.h"

#include "Network/RestDispatcher.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QUrl>
#include <QtWidgets/QMenu>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QMessageBox>

#include "Events/LocalDispatcher.h"

#include "APIClient/NotificationService.h"

namespace Drive
{

AppController& AppController::instance()
{
    static AppController myself;
    return myself;
}

AppController::AppController(QWidget *parent)
    : QMainWindow(parent)
    , currentState(NotAuthorized)
    , currentAuthToken(QString())
{
    createActions();
    createTrayIcon();
    createSettingsWidget();

    QMetaObject::connectSlotsByName(this);

    TrayIcon::instance().setState(currentState);

    LoginController& loginController = LoginController::instance();

    connect(&loginController, SIGNAL(loginFinished()),
        this, SLOT(onLoginFinished()));

    on_settingsWidget_logout();
}

State AppController::state() const
{
    return currentState;
}

QString AppController::authToken() const
{
    return currentAuthToken;
}

void AppController::setAuthToken(const QString &token)
{
    currentAuthToken = token;
    GeneralRestDispatcher::instance().setAuthToken(token);
}

ProfileData AppController::profileData() const
{
    return currentProfileData;
}

void AppController::setProfileData(const ProfileData& data)
{
    currentProfileData = data;
    emit profileDataUpdated(currentProfileData);
}


void AppController::createActions()
{
    actionOpenFolder = new QAction(tr("Open Folder"), this);
    actionOpenFolder->setObjectName("actionOpenFolder");
    actionOpenFolder->setIcon(QIcon(":/icons/open.png"));

    actionPause = new QAction(tr("Pause Sync"), this);
    actionPause->setObjectName("actionPause");
    actionPause->setIcon(QIcon(":/icons/pause.png"));

    actionResume = new QAction(tr("Resume Sync"), this);
    actionResume->setObjectName("actionResume");
    actionResume->setIcon(QIcon(":/icons/resume.png"));

    actionPreferences = new QAction(tr("Preferences..."), this);
    actionPreferences->setObjectName("actionPreferences");
    actionPreferences->setIcon(QIcon(":/icons/preferences.png"));

    actionExit = new QAction(tr("Exit"), this);
    actionExit->setObjectName("actionExit");
}

void AppController::createTrayIcon()
{
    TrayIcon &trayIcon = TrayIcon::instance();
    trayIcon.setObjectName("trayIcon");
    
    trayMenu = new QMenu(this);
    
    trayMenu->addAction(actionOpenFolder);
    trayMenu->addAction(actionPreferences);
    trayMenu->addSeparator();
    trayMenu->addAction(actionPause);
    trayMenu->addSeparator();
    trayMenu->addAction(actionExit);

    trayMenu->setDefaultAction(actionOpenFolder);

    trayIcon.setContextMenu(trayMenu);    

    connect(this, SIGNAL(stateChanged(Drive::State)),
        &trayIcon, SLOT(setState(Drive::State)));

    connect(&trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
        this, SLOT(on_trayIcon_activated(QSystemTrayIcon::ActivationReason)));
    
    trayIcon.show();
}

void AppController::createSettingsWidget()
{
    //settingsWidget = QSharedPointer<SettingsWidget>(new SettingsWidget());

    SettingsWidget& settingsWidget = SettingsWidget::instance();

    settingsWidget.setObjectName("settingsWidget");
    
    settingsWidget.setWindowTitle(
        QString("%1 %2 %3")
        .arg(Strings::companyName)
        .arg(Strings::appName)
        .arg(tr("Preferences")));
    
    settingsWidget.setWindowIcon(QIcon(":/icons/preferences.png"));

    connect(&settingsWidget, SIGNAL(openFolder()),
        actionOpenFolder, SLOT(trigger()));

    connect(&settingsWidget, SIGNAL(logout()),
        this, SLOT(on_settingsWidget_logout()));

    connect(this, SIGNAL(profileDataUpdated(ProfileData)), 
        &settingsWidget, SLOT(onProfileDataUpdated(ProfileData)));
}

void AppController::setState(State newState)
{
    if (currentState != newState)
    {
        currentState = newState;
        emit stateChanged(currentState);
    }    
}

void AppController::on_actionOpenFolder_triggered()
{
    QDesktopServices::openUrl(
        QUrl(QString("file:///%1").arg(
            Settings::instance().get(Settings::folderPath).toString())
        , QUrl::TolerantMode));
}

void AppController::on_actionPause_triggered()
{
    trayMenu->removeAction(actionPause);
    trayMenu->insertAction(actionExit, actionResume);
    trayMenu->insertSeparator(actionExit);
}

void AppController::on_actionResume_triggered()
{
    trayMenu->removeAction(actionResume);
    trayMenu->insertAction(actionExit, actionPause);
    trayMenu->insertSeparator(actionExit);
}

void AppController::on_actionPreferences_triggered()
{
    SettingsWidget &settingsWidget = SettingsWidget::instance();
    
    settingsWidget.show();
    
    settingsWidget.setWindowState(
        (settingsWidget.windowState() & ~Qt::WindowMinimized)
        | Qt::WindowActive);
    
    settingsWidget.raise();  // for MacOS
    settingsWidget.activateWindow(); // for Windows
}

void AppController::on_actionExit_triggered()
{
    QLOG_TRACE() << "Exiting";
    LoginController::instance().closeAll();
    close();
}

void AppController::on_trayIcon_activated(QSystemTrayIcon::ActivationReason reason)
{
    QLOG_TRACE() << "ActivationReason:" << reason;
    
    if (reason == QSystemTrayIcon::DoubleClick)
        actionOpenFolder->trigger();

//     if (reason == QSystemTrayIcon::Trigger)
//         QMessageBox::information(0, "", "test");
}

void AppController::on_settingsWidget_logout()
{
    SettingsWidget::instance().hide();    
    LoginController::instance().showLoginForm();
}

void AppController::onLoginFinished()
{
    setState(State::Syncing);
    FileSystemHelper::setWindowsFolderIcon(
        Settings::instance().get(Settings::folderPath).toString(), 1);

	LocalFileEventDispatcher *localDisp = new LocalFileEventDispatcher(this);

	NotificationResourceRef notifier = NotificationResource::create();
	notifier->listenRemoteFileEvents();
}

}