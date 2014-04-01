#include "singleapp.h"

#include "settings/settings.h"

#include <QPointer>
#include <QProcess>

#include <QsLog/QsLog.h>

#include "Application/TrayIcon.h"

SingleApplication::SingleApplication(int argc, char *argv[])
	: QApplication(argc, argv)
	, m_trayIcon(nullptr)
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
	{
		server->terminate();

		if (Drive::Settings::instance().get(Drive::Settings::forceRelogin).toBool())
		{
			qint64 pid = 0;
			QLOG_DEBUG() << "Restarting...";
			if (!QProcess::startDetached(
					QApplication::applicationFilePath(),
					qApp->arguments(),
					QApplication::applicationDirPath(),
					&pid))
			{
				QLOG_ERROR() << "Starting of new instance FAILED.";
			}
			else
			{
				QLOG_DEBUG() << "New instance started (" << pid << ").";
			}
		}
	}

}

bool SingleApplication::shouldContinue()
{
	return _shouldContinue;
}

QPointer<Drive::TrayIcon> SingleApplication::trayIcon()
{
	if (m_trayIcon == nullptr)
	{
		m_trayIcon = new Drive::TrayIcon(this);
		m_trayIcon->setObjectName("trayIcon");
	}
	return m_trayIcon;
}

void SingleApplication::slotShowUp()
{
	emit showUp();
}
