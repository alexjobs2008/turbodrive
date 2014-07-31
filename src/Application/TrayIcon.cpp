#include "TrayIcon.h"

#include "Util/AppStrings.h"
#include "QsLog/QsLog.h"

#include <QtCore/QCoreApplication>

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
	AnimatedSystemTrayIcon::State *state;

	// NotAuthorized

	state = new AnimatedSystemTrayIcon::State(stateToString(NotAuthorized),
		QPixmap(":/tray_icons/no_auth.png"));

	statesMap.insert(NotAuthorized, state);

	// Authorizing

	state = new AnimatedSystemTrayIcon::State(stateToString(Authorizing),
		QPixmap(":/tray_icons/auth_0001.png"));

	state->frames
		<< QPixmap(":/tray_icons/auth_0002.png");

	state->delay = 333;

	statesMap.insert(Authorizing, state);

	// Syncing

	state = new AnimatedSystemTrayIcon::State(stateToString(Syncing),
		QPixmap(":/tray_icons/syncing_0001.png"));

	state->frames
		<< QPixmap(":/tray_icons/syncing_0002.png")
		<< QPixmap(":/tray_icons/syncing_0003.png")
		<< QPixmap(":/tray_icons/syncing_0004.png");

	state->delay = 333;

	statesMap.insert(Syncing, state);

	// Synced

	state = new AnimatedSystemTrayIcon::State(stateToString(Synced),
		QPixmap(":/tray_icons/synced.png"));

	statesMap.insert(Synced, state);

	// Paused

	state = new AnimatedSystemTrayIcon::State(stateToString(Paused),
		QPixmap(":/tray_icons/paused.png"));

	statesMap.insert(Paused, state);

	// Error

	state = new AnimatedSystemTrayIcon::State(stateToString(Error),
		QPixmap(":/tray_icons/error.png"));

	statesMap.insert(Error, state);

	QMapIterator<Drive::State, AnimatedSystemTrayIcon::State*> i(statesMap);
	while (i.hasNext()) {
		i.next();
		appendState(i.value());
	}
}

}
