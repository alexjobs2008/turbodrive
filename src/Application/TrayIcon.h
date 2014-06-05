#ifndef APP_TRAY_ICON_H
#define APP_TRAY_ICON_H

#include "types.h"
#include "AnimatedSystemTrayIcon/AnimatedSystemTrayIcon.h"

#include <QtCore/QMap>

namespace Drive
{


class TrayIcon : public AnimatedSystemTrayIcon
{
	Q_OBJECT
public:
	explicit TrayIcon(QObject *parent);

public slots:
	void setState(Drive::State state);
	void onProcessingProgress(int, int);

	Drive::State getState() const;

private:
	Q_DISABLE_COPY(TrayIcon)
	void loadStates();

	QMap<Drive::State, AnimatedSystemTrayIcon::State*> statesMap;
	Drive::State _state;
	QString baseToolTip;
};

}


#endif
