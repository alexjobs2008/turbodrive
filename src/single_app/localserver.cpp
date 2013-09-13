#include "localserver.h"

#include <QFile>
#include <QStringList>

LocalServer::LocalServer()
{
}

LocalServer::~LocalServer()
{
    server->close();
    for(int i = 0; i < clients.size(); ++i)
        clients[i]->close();
}

void LocalServer::run()
{
    server = new QLocalServer();

    QObject::connect(server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    QObject::connect(this, SIGNAL(privateDataReceived(QString)), this, SLOT(slotOnData(QString)));

#ifdef Q_OS_UNIX
    QFile address(QString("/tmp/" LOCAL_SERVER_NAME));
    if(address.exists()){
      address.remove();
    }
#endif

    QString serverName = QString(LOCAL_SERVER_NAME);  
    server->listen(serverName);
    while(server->isListening() == false){
      server->listen(serverName);
      msleep(100);
    }
    exec();
}

void LocalServer::exec()
{
    while(server->isListening())
    {
      msleep(100);
      server->waitForNewConnection(100);
      for(int i = 0; i < clients.size(); ++i)
      {
        if(clients[i]->waitForReadyRead(100))
        {
            QByteArray data = clients[i]->readAll();
          emit privateDataReceived(data);
        }
      }
    }
}


void LocalServer::slotNewConnection()
{
    clients.push_front(server->nextPendingConnection());
}


void LocalServer::slotOnData(QString data)
{
    if(data.contains("CMD:", Qt::CaseInsensitive)){
        onSGC(data);
    }
    else
    {
        emit dataReceived(data);
    }
}


void LocalServer::onCMD(QString data)
{
    data.replace(0, 4, "");

    QStringList commands;
    commands << "showUp";

    switch(commands.indexOf(data)){
    case 0:
        emit showUp();
    }
}
