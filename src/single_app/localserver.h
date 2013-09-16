#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <QtCore/QThread>
#include <QtCore/QVector>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

class LocalServer : public QThread
{
    Q_OBJECT
public:
    LocalServer();
    ~LocalServer();
    void shut();

protected:
    void run();
    void exec();

signals:
    void dataReceived(QString data);
    void privateDataReceived(QString data);
    void showUp();

private slots:
    void slotNewConnection();
    void slotOnData(QString data);

private:
    QLocalServer* server;
    QVector<QLocalSocket*> clients;
    void onCMD(QString data);
};

#endif // LOCALSERVER_H
