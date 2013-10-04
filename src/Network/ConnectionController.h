#ifndef CONNECTION_CONTROLLER_H
#define CONNECTION_CONTROLLER_H

#include <QtCore/QObject>

class ConnectionController : public QObject
{
    Q_OBJECT
public:

    enum State {
        Disconnected = 0,
        Connecting,
        Connected
    };

    ConnectionController(QObject *parent = 0);

signals:

public slots:

private:
    State state;

};

#endif CONNECTION_CONTROLLER_H