#include "TrayIcon.h"

#include "Util/AppStrings.h"
#include "QsLog/QsLog.h"

#include <QtCore/QCoreApplication>
#include <QScreen>
#include <QGuiApplication>

#include "FileUtils.h"
#include "Settings/settings.h"

namespace Drive
{

void TrayIcon::setState(Drive::State state)
{
	if (statesMap.contains(state))
	{
		_state = state;
		AnimatedSystemTrayIcon::setState(statesMap.value(state)->name);

		baseToolTip = QString("%1")
			.arg(stateToString(state));

		setToolTip(baseToolTip);

        QString homePath = Settings::instance().get(Settings::folderPath).toString();

        switch (state)
        {
        case Drive::NotAuthorized:
            FileSystemHelper::setFolderIcon(homePath, FOLDER_ICON_SYNC);
            FolderIconController::instance().setState(homePath, FOLDER_ICON_SYNC);
            break;
        case Drive::Authorizing:
            FileSystemHelper::setFolderIcon(homePath, FOLDER_ICON_SYNC);
            FolderIconController::instance().setState(homePath, FOLDER_ICON_SYNC);
            break;
        case Drive::Syncing:
            FileSystemHelper::setFolderIcon(homePath, FOLDER_ICON_SYNC);
            FolderIconController::instance().setState(homePath, FOLDER_ICON_SYNC);
            break;
        case Drive::Synced:
            FileSystemHelper::setFolderIcon(homePath, FOLDER_ICON_OK);
            FolderIconController::instance().setState(homePath, FOLDER_ICON_OK);
            break;
        case Drive::Paused:
            FileSystemHelper::setFolderIcon(homePath, FOLDER_ICON_SYNC);
            FolderIconController::instance().setState(homePath, FOLDER_ICON_SYNC);
            break;
        case Drive::Error:
            FileSystemHelper::setFolderIcon(homePath, FOLDER_ICON_ERROR);
            FolderIconController::instance().setState(homePath, FOLDER_ICON_ERROR);
            break;
        }
    }
}

void TrayIcon::onProcessingProgress(int currentPos, int totalEventCount)
{
	if (_state == Drive::Syncing)
	{
		setToolTip(QString(tr("%1 (%2 of %3)"))
			.arg(baseToolTip)
			.arg(currentPos)
			.arg(totalEventCount));
	}
}

Drive::State TrayIcon::getState() const
{
	return _state;
}

TrayIcon::TrayIcon(QObject *parent)
	: AnimatedSystemTrayIcon(parent)
{
	loadStates();
}

void TrayIcon::loadStates()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    qreal dotsPerInch = screen->physicalDotsPerInch();
    bool isRetinaDisplay = dotsPerInch > 200;

    const char *no_auth_img = isRetinaDisplay ? ":/tray_icons/32/no_auth.png" : ":/tray_icons/16/no_auth.png";
    const char *auth_0001_img = isRetinaDisplay ? ":/tray_icons/32/auth_0001.png" : ":/tray_icons/16/auth_0001.png";
    const char *auth_0002_img = isRetinaDisplay ? ":/tray_icons/32/auth_0002.png" : ":/tray_icons/16/auth_0002.png";
    const char *syncing_0001_img = isRetinaDisplay ? ":/tray_icons/32/syncing_0001.png" : ":/tray_icons/16/syncing_0001.png";
    const char *syncing_0002_img = isRetinaDisplay ? ":/tray_icons/32/syncing_0002.png" : ":/tray_icons/16/syncing_0002.png";
    const char *syncing_0003_img = isRetinaDisplay ? ":/tray_icons/32/syncing_0003.png" : ":/tray_icons/16/syncing_0003.png";
    const char *syncing_0004_img = isRetinaDisplay ? ":/tray_icons/32/syncing_0004.png" : ":/tray_icons/16/syncing_0004.png";
    const char *synced_img = isRetinaDisplay ? ":/tray_icons/32/synced.png" : ":/tray_icons/16/synced.png";
    const char *paused_img = isRetinaDisplay ? ":/tray_icons/32/paused.png" : ":/tray_icons/16/paused.png";
    const char *error_img = isRetinaDisplay ? ":/tray_icons/32/error.png" : ":/tray_icons/16/error.png";

	AnimatedSystemTrayIcon::State *state;

	// NotAuthorized

	state = new AnimatedSystemTrayIcon::State(stateToString(NotAuthorized),
        QPixmap(no_auth_img));

	statesMap.insert(NotAuthorized, state);

	// Authorizing

	state = new AnimatedSystemTrayIcon::State(stateToString(Authorizing),
        QPixmap(auth_0001_img));

	state->frames
        << QPixmap(auth_0002_img);

	state->delay = 333;

	statesMap.insert(Authorizing, state);

	// Syncing

	state = new AnimatedSystemTrayIcon::State(stateToString(Syncing),
        QPixmap(syncing_0001_img));

	state->frames
        << QPixmap(syncing_0002_img)
        << QPixmap(syncing_0003_img)
        << QPixmap(syncing_0004_img);

	state->delay = 333;

	statesMap.insert(Syncing, state);

	// Synced

	state = new AnimatedSystemTrayIcon::State(stateToString(Synced),
        QPixmap(synced_img));

	statesMap.insert(Synced, state);

	// Paused

	state = new AnimatedSystemTrayIcon::State(stateToString(Paused),
        QPixmap(paused_img));

	statesMap.insert(Paused, state);

	// Error

	state = new AnimatedSystemTrayIcon::State(stateToString(Error),
        QPixmap(error_img));

	statesMap.insert(Error, state);

	QMapIterator<Drive::State, AnimatedSystemTrayIcon::State*> i(statesMap);
	while (i.hasNext()) {
		i.next();
		appendState(i.value());
	}
}

}
