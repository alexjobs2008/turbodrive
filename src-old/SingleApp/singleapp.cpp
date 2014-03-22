#include "singleapp.h"

SingleApplication::SingleApplication(int argc, char *argv[])
    : QApplication(argc, argv)
{
    _shouldContinue = false;

    socket = new QLocalSocket();

    // Attempts to connect to the LocalServer
    socket->connectToServer(LOCAL_SERVER_NAME);
    if(socket->waitForConnected(100)){
        socket->write("CMD:showUp");
        socket->flush();
        QThread::msleep(100);
        socket->close();
    } else {
    // Connection failed, so continuing the execution
        _shouldContinue = true;
        server = new LocalServer();
       server->start();
       QObject::connect(server, SIGNAL(showUp()), this, SLOT(slotShowUp()));
    }
}

SingleApplication::~SingleApplication()
{
    if(_shouldContinue)
        server->terminate();    
}

bool SingleApplication::shouldContinue()
{
    return _shouldContinue;
}


void SingleApplication::slotShowUp()
{
    emit showUp();
}
