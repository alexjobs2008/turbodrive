#include "AnimatedSystemTrayIcon.h"

#include <QtCore/QTimerEvent>

AnimatedSystemTrayIcon& AnimatedSystemTrayIcon::instance()
{
    static AnimatedSystemTrayIcon myself;
    return myself;
}

AnimatedSystemTrayIcon::AnimatedSystemTrayIcon(QObject *parent)
	: QSystemTrayIcon(parent)
	, currentState(0)
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

	if (!currentState)	
		setState(state->name);
}

bool AnimatedSystemTrayIcon::setState(const QString &stateName)
{
	if (!states.contains(stateName))
		return false;

    if (currentState && currentState->name == stateName)
        return true;

    if (timerId)
    {
        killTimer(timerId);
        timerId = 0;
    }
    
    currentState = states.value(stateName);
    currentState->currentFrame = 0;
	setIcon(QIcon(currentState->frames.at(currentState->currentFrame)));

    if (currentState->frames.size() > 1)    
        timerId = this->startTimer(currentState->delay);
    
    return true;
}

void AnimatedSystemTrayIcon::timerEvent(QTimerEvent *event)
{
	if (!currentState)
	{
		killTimer(timerId);
        timerId = 0;
		return;
	}
    
    if (++currentState->currentFrame > currentState->frames.size() - 1)
        currentState->currentFrame = 0;

    setIcon(QIcon(currentState->frames.at(currentState->currentFrame)));
}