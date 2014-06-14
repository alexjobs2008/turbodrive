#include "types.h"

namespace Drive
{

QString stateToString(State state)
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
