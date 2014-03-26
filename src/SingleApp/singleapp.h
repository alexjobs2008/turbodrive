#ifndef SINGLE_APP_H
#define SINGLE_APP_H

#include "localserver.h"

#include <QtWidgets/QApplication>
#include <QtNetwork/QLocalSocket>

namespace Drive
{

class TrayIcon;

}

class SingleApplication : public QApplication
{
	Q_OBJECT
public:
	explicit SingleApplication(int, char *[]);
	~SingleApplication();
	bool shouldContinue();
	QPointer<Drive::TrayIcon> trayIcon();

signals:
	void showUp();

private slots:
	void slotShowUp();

private:
	QLocalSocket* socket;
	LocalServer* server;

	Drive::TrayIcon* m_trayIcon;

	bool _shouldContinue;
};

#endif // SINGLE_APP_H
