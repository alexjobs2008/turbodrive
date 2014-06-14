#ifndef TYPES_H
#define TYPES_H

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

QString stateToString(State state);

}

#endif
