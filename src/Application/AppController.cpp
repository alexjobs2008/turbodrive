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

#include "Events/LocalFileEventNotifier.h"
#include "Events/FileEventDispatcher.h"
#include "Events/Syncer.h"
#include "Events/Cache.h"

#include "APIClient/NotificationService.h"
#include "APIClient/FilesService.h"

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

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
    , syncer(0)
{
    createActions();
    createTrayIcon();
    createSettingsWidget();

    QMetaObject::connectSlotsByName(this);

    TrayIcon::instance().setState(currentState);

    LoginController& loginController = LoginController::instance();
    connect(&loginController, SIGNAL(loginFinished()),
        this, SLOT(onLoginFinished()));

    SettingsWidget::instance().hide();
    LoginController::instance().showLoginFormOrLogin();    
}

AppController::~AppController()
{
//     if (syncer) delete syncer;
//     if (localCache) delete localCache;
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
    GeneralRestDispatcher::instance().setWorkspaceId(data.defaultWorkspace().id);
    emit profileDataUpdated(currentProfileData);
}

const QString& AppController::serviceChannel() const
{
    return profileData().defaultWorkspace().serviceNotificationChannel();
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

    connect(this, SIGNAL(processingProgress(int, int)),
        &trayIcon, SLOT(onProcessingProgress(int, int)));

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
        QString("%1 %2")
        .arg(Strings::getAppString(Strings::AppFullName))
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

    FileEventDispatcher::instance().pause();
    LocalFileEventNotifier::instance().stop();


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
    
    FileEventDispatcher::instance().cancelAll();
    LocalFileEventNotifier::instance().stop();
    LocalCache::instance().clear();
}

void AppController::onLoginFinished()
{
    setState(State::Synced);

	createFolder();

    FileSystemHelper::setWindowsFolderIcon(
        Settings::instance().get(Settings::folderPath).toString(), 1);

    FileEventDispatcher& eventDispatcher = FileEventDispatcher::instance();

    connect(&eventDispatcher, SIGNAL(processing()),
        this, SLOT(onQueueProcessing()));

    connect(&eventDispatcher, SIGNAL(finished()),
        this, SLOT(onQueueFinished()));

    connect(&eventDispatcher, SIGNAL(progress(int, int)),
        this, SLOT(onProcessingProgress(int, int)));

	LocalFileEventNotifier& localNotifier = LocalFileEventNotifier::instance();

    connect(&localNotifier, SIGNAL(newLocalFileEvent(LocalFileEvent)),
        &eventDispatcher, SLOT(addLocalFileEvent(LocalFileEvent)));

    NotificationResourceRef remoteNotifier = NotificationResource::create();
    
    connect(remoteNotifier.data(), SIGNAL(newRemoteFileEvent(RemoteFileEvent)),
        &eventDispatcher, SLOT(addRemoteFileEvent(RemoteFileEvent)));

    if (!syncer)
    {
        syncer = new Syncer(this);
    }

    LocalCache &localCache = LocalCache::instance();

    connect(syncer, SIGNAL(newFileDesc(Drive::RemoteFileDesc))
        , &localCache, SLOT(onNewFileDesc(Drive::RemoteFileDesc)));
    
    connect(syncer, SIGNAL(newRemoteEvent(RemoteFileEvent)),
        &eventDispatcher, SLOT(addRemoteFileEvent(RemoteFileEvent)));

    connect(syncer, SIGNAL(newLocalEvent(LocalFileEvent)),
        &eventDispatcher, SLOT(addLocalFileEvent(LocalFileEvent)));    

    syncer->fullSync();

    localNotifier.setFolder(); // and start listen for local file events
	remoteNotifier->listenRemoteFileEvents();
}

void AppController::onQueueProcessing()
{
    setState(Drive::Syncing);
}

void AppController::onQueueFinished()
{
    setState(Drive::Synced);
}

void AppController::onProcessingProgress(int currentPos, int totalEvents)
{
    emit processingProgress(currentPos, totalEvents);
}

void AppController::createFolder()
{
	QString folderPath =
		Settings::instance().get(Settings::folderPath).toString();

    QDir dir;
    dir.mkpath(folderPath);
}

}