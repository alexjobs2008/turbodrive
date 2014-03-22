#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <QtCore/QObject>

namespace Drive
{

enum State {
	NotAuthorized = 0,
	Authorizing,
	Syncing,
	Synced,
	Paused,
	Error
};

static QString stateToString(State state)
{
	switch (state)
	{
	case NotAuthorized: return QObject::tr("Not authorized");
	case Authorizing: return QObject::tr("Signing in...");
	case Syncing: return QObject::tr("Syncing...");
	case Synced: return QObject::tr("Up to date");
	case Paused: return QObject::tr("Syncing paused");
	case Error: return QObject::tr("Error");
	default: return QString();
	}
};

}

#endif