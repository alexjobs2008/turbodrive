#ifndef SINGLE_APP_H
#define SINGLE_APP_H

#include "localserver.h"

#include <QGuiApplication>
#include <QLocalSocket>

class SingleApplication : public QGuiApplication
{
    Q_OBJECT
public:
    explicit SingleApplication(int, char *[]);
    ~SingleApplication();
    bool shouldContinue();

signals:
    void showUp();

private slots:
    void slotShowUp();
  
private:
    QLocalSocket* socket;
    LocalServer* server;
    bool _shouldContinue;
};

#endif // SINGLE_APP_H
