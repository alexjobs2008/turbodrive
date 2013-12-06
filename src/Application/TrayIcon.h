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
    static TrayIcon& instance();

public slots:
    void setState(Drive::State state);
    Drive::State getState() const;

private:
    Q_DISABLE_COPY(TrayIcon)
    explicit TrayIcon(QObject *parent = 0);
    void loadStates();

    QMap<Drive::State, AnimatedSystemTrayIcon::State*> statesMap;
    Drive::State _state;
};

}


#endif