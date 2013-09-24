#include "mainwindow.h"
#include "AnimatedSystemTrayIcon/AnimatedSystemTrayIcon.h"
#include "Settings/settings.h"
#include "SettingsUI/SettingsWidget.h"
#include "QsLog/QsLog.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStandardPaths>
#include <QtCore/QUrl>
#include <QtWidgets/QMenu>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    
    createActions();
    createTrayIcon();
    createSettingsWidget();

    QMetaObject::connectSlotsByName(this);
}

MainWindow::~MainWindow()
{
    delete settingsWidget;
}

void MainWindow::createActions()
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

void MainWindow::createTrayIcon()
{
    trayIcon = new AnimatedSystemTrayIcon(this);
    trayIcon->setObjectName("trayIcon");
    
    trayIcon->appendState(new AnimatedSystemTrayIcon::
        State("Base", QPixmap(":/icons/tray_base.png")));

    trayMenu = new QMenu(this);
    
    trayMenu->addAction(actionOpenFolder);
    trayMenu->addAction(actionPreferences);
    trayMenu->addSeparator();
    trayMenu->addAction(actionPause);
    trayMenu->addSeparator();
    trayMenu->addAction(actionExit);

    trayMenu->setDefaultAction(actionOpenFolder);

    trayIcon->setContextMenu(trayMenu);

    trayIcon->setToolTip(QCoreApplication::applicationName());

    trayIcon->show();
}

void MainWindow::createSettingsWidget()
{
    settingsWidget = new SettingsWidget();
    
    settingsWidget->setWindowTitle(
        QString("%1 %2 %3")
        .arg(localizedOrganizationName)
        .arg(localizedApplicationName)
        .arg(tr("Preferences")));
    
    settingsWidget->setWindowIcon(QIcon(":/icons/preferences.png"));
}

void MainWindow::on_actionOpenFolder_triggered()
{
    QString homePath =
        QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0);
    
    QDesktopServices::openUrl(
        QUrl(QString("file:///%1").arg(homePath), QUrl::TolerantMode));
}

void MainWindow::on_actionPause_triggered()
{
    trayMenu->removeAction(actionPause);
    trayMenu->insertAction(actionExit, actionResume);
    trayMenu->insertSeparator(actionExit);
}

void MainWindow::on_actionResume_triggered()
{
    trayMenu->removeAction(actionResume);
    trayMenu->insertAction(actionExit, actionPause);
    trayMenu->insertSeparator(actionExit);
}

void MainWindow::on_actionPreferences_triggered()
{
    settingsWidget->show();
    
    settingsWidget->setWindowState(
        (settingsWidget->windowState() & ~Qt::WindowMinimized)
        | Qt::WindowActive);
    
    settingsWidget->raise();  // for MacOS
    settingsWidget->activateWindow(); // for Windows
}

void MainWindow::on_actionExit_triggered()
{
    QLOG_TRACE() << "Exiting";
    close();
}

void MainWindow::on_trayIcon_activated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick)
        actionOpenFolder->trigger();

    if (reason == QSystemTrayIcon::Trigger)
        QMessageBox::information(0, "", "test");
}