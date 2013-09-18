#include "mainwindow.h"
#include "animated_system_tray_icon.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , counter(0)
{
	trayIcon = new AnimatedSystemTrayIcon(this);

	AnimatedSystemTrayIcon::State *animatedState = new AnimatedSystemTrayIcon::
        State("AnimatedState", QPixmap(":/img/ice-cream2.png"));
    
    animatedState->frames
        << QPixmap(":/img/ice-cream3.png")
	    << QPixmap(":/img/ice-cream4.png")
	    << QPixmap(":/img/ice-cream5.png")
	    << QPixmap(":/img/ice-cream6.png")
	    << QPixmap(":/img/ice-cream7.png");

    animatedState->delay = 100;
	trayIcon->appendState(animatedState);
	trayIcon->show();

    startTimer(5000);
}

MainWindow::~MainWindow()
{
}

void MainWindow::timerEvent(QTimerEvent * event)
{
    if (++counter == 1)
    {
        trayIcon->appendState(new AnimatedSystemTrayIcon::
            State("StaticImageState", QPixmap(":/img/ice-cream1.png")));

        trayIcon->setState("StaticImageState");
        startTimer(5000);
    }
    else if (counter == 2)
    {
        trayIcon->setState("AnimatedState");
    }
}