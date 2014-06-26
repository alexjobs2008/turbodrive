#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class AnimatedSystemTrayIcon;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	void timerEvent(QTimerEvent*);

private:
	AnimatedSystemTrayIcon *trayIcon;
	int counter;
};

#endif // MAINWINDOW_H
