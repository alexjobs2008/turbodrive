﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "types.h"
#include "Events/Syncer.h"
#include "APIClient/ApiTypes.h"

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSystemTrayIcon>
#include <QtCore/QSharedPointer>
#include <QPointer>
#include <QtWidgets/QMenu>
#include <QtGui/QShowEvent>

#include <memory>

#include "TrayIcon.h"

class QMenu;
class QAction;

namespace Drive
{

class SettingsWidget;
class Syncer;
class LocalCache;
class RemoteConfig;


class TrayMenu: public QMenu
{
    Q_OBJECT
    bool show;
public:
    TrayMenu(QWidget *parent = 0) : QMenu(parent), show(true) {}
    void setShow(bool show) { this->show = show; }
    bool isShow() { return show; }
    void showEvent(QShowEvent *event);
};

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

	const QString serviceChannel() const;

	void setTrayIcon(const QPointer<TrayIcon>& trayIcon);

	void createFolder();

	void onSettingChanged(const QString& settingName, QVariant, QVariant);

	void restart();
	void restartRemotesOnly();

public slots:
	void setState(State newState);
    void setStateText();
    void exitApplication();
    void enableContextMenu(bool enabled);

signals:
	void stateChanged(Drive::State state);
	void processingProgress(int, int);
	void profileDataUpdated(const ProfileData& data);
    void tutorial();
    void login();

private slots:
	void on_actionOpenFolder_triggered();
	void on_actionOpenWebSite_triggered();
	void on_actionLogout_triggered();
	void on_actionPause_triggered();
	void on_actionResume_triggered();
	void on_actionPreferences_triggered();
	void on_actionUpdate_triggered();
	void on_actionHelp_triggered();
	void on_actionAbout_triggered();
	void on_actionExit_triggered();

	void on_trayIcon_messageClicked();
    void on_trayIcon_activated(QSystemTrayIcon::ActivationReason reason);
    void on_settingsWidget_logout();

    void onTutorial();
    void onShowLogin();
	void onLoginFinished();
	void onQueueProcessing();
	void onQueueFinished();

	void onProcessingProgress(int, int);

	void onUpdate(const QString& version);

private:
	Q_DISABLE_COPY(AppController)
	explicit AppController(QWidget *parent = nullptr);

	void createActions();
	void createTrayIcon();
	void createSettingsWidget();

	void downloadUpdate();

	void onLoginFinishedImpl(bool restartFSWatcher);


	QPointer<TrayIcon> m_trayIcon;

    /* TrayMenu */ QMenu  *trayMenu;

	QAction *actionStatus;
	QAction *actionOpenFolder;
	QAction *actionOpenWebSite;
	QAction *actionUsername;
	QAction *actionLogout;
	QAction *actionPause;
	QAction *actionResume;
	QAction *actionPreferences;
	QAction *actionDownloadUpdate;
	QAction *actionHelp;
	QAction *actionAbout;
	QAction *actionExit;

	State currentState;

	ProfileData currentProfileData;
	QString currentAuthToken;

	std::unique_ptr<Syncer> m_syncer;
	std::unique_ptr<RemoteConfig> m_remoteConfig;
};

}

#endif // MAINWINDOW_H
