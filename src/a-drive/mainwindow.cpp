#include "mainwindow.h"
#include "animated_system_tray_icon/animated_system_tray_icon.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	AnimatedSystemTrayIcon *trayIcon = new AnimatedSystemTrayIcon(this);

	AnimatedSystemTrayIcon::State *state1 =
		new AnimatedSystemTrayIcon::State("State1", QPixmap(":/temp/ice-cream1.png"));

	trayIcon->appendState(state1);

	trayIcon->show();
}

MainWindow::~MainWindow()
{

}
