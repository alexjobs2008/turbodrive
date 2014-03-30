#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "types.h"
#include "APIClient/ApiTypes.h"

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSystemTrayIcon>
#include <QtCore/QSharedPointer>
#include <QPointer>

#include "TrayIcon.h"

class QMenu;
class QAction;

namespace Drive
{

class SettingsWidget;
class Syncer;
class LocalCache;

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

	const QString& serviceChannel() const;

	void setTrayIcon(const QPointer<TrayIcon>& trayIcon);

	void createFolder();

public slots:
	void setState(State newState);

signals:
	void stateChanged(Drive::State state);
	void processingProgress(int, int);
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
	void onQueueProcessing();
	void onQueueFinished();

	void onProcessingProgress(int, int);

private:
	Q_DISABLE_COPY(AppController)
	explicit AppController(QWidget *parent = 0);
	~AppController();

	void createActions();
	void createTrayIcon();
	void createSettingsWidget();

	QPointer<TrayIcon> m_trayIcon;

	QMenu *trayMenu;

	QAction *actionOpenFolder;
	QAction *actionPause;
	QAction *actionResume;
	QAction *actionPreferences;
	QAction *actionExit;

	State currentState;

	ProfileData currentProfileData;
	QString currentAuthToken;
	Syncer *syncer;
};

}

#endif // MAINWINDOW_H
