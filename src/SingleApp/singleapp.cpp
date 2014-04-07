#include "singleapp.h"

#include "settings/settings.h"
#include "Application/TrayIcon.h"

#include <QLocalSocket>
#include <QPointer>
#include <QProcess>

#include <QsLog/QsLog.h>

SingleApplication::SingleApplication(int argc, char *argv[])
	: QApplication(argc, argv)
	, m_shouldContinue(false)
	, m_localServer(nullptr)
	, m_trayIcon(nullptr)
{
	QLocalSocket socket;
	socket.connectToServer(LOCAL_SERVER_NAME);
	if (!socket.waitForConnected(500))
	{
		m_shouldContinue = true;
		m_localServer = new QLocalServer(this);
		connect(m_localServer, &QLocalServer::newConnection,
				this, &SingleApplication::showUp);
		m_localServer->listen(LOCAL_SERVER_NAME);
	}
}

SingleApplication::~SingleApplication()
{
	if(m_shouldContinue)
	{
		m_localServer->close();

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
	return m_shouldContinue;
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
