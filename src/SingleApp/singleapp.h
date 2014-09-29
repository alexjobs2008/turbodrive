#ifndef SINGLE_APP_H
#define SINGLE_APP_H

#include <QtWidgets/QApplication>
#include <QtNetwork/QLocalServer>

namespace Drive
{
class TrayIcon;
}

class SingleApplication: public QApplication
{
	Q_OBJECT
private:
    static SingleApplication* appInstance;

public:
    SingleApplication(int argc, char *argv[]);
	~SingleApplication();

    static SingleApplication& instance() { return *appInstance; }

	bool shouldContinue();
	Q_SIGNAL void showUp();

	QPointer<Drive::TrayIcon> trayIcon();

private:
	const QStringList m_arguments;

	bool m_shouldContinue;
	QLocalServer* m_localServer;
	Drive::TrayIcon* m_trayIcon;
};

#endif // SINGLE_APP_H
