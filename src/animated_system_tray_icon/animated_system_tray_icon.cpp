#include "animated_system_tray_icon.h"

#include <QtCore/QTimerEvent>

AnimatedSystemTrayIcon::AnimatedSystemTrayIcon(QObject *parent)
	: QSystemTrayIcon(parent)
	, currentState(QString())
	, timerId(0)
{
}

AnimatedSystemTrayIcon::~AnimatedSystemTrayIcon()
{
	qDeleteAll(states);
}

void AnimatedSystemTrayIcon::appendState(State *state)
{
	if (states.contains(state->name))
	{
		if (timerId)
		{
			killTimer(timerId);
			timerId = 0;
		}
		
		State *oldState = states.take(state->name);
		delete oldState;
	}	
	
	states.insert(state->name, state);

	if (currentState.isNull())	
		setState(state->name);
}

bool AnimatedSystemTrayIcon::setState(const QString &stateName)
{
	if (!states.contains(stateName))
		return false;
	
	State *currentState = states.value(stateName);
	
	QIcon icon(currentState->frames.at(0));
	setIcon(icon);
	
	return true;
}

void AnimatedSystemTrayIcon::timerEvent(QTimerEvent *event)
{
	if (currentState.isEmpty())
	{
		killTimer(timerId);
		return;
	}
}