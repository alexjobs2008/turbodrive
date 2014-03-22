#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSystemTrayIcon>
#include <QtCore/QSharedPointer>

class SettingsWidget;
class QMenu;
class QAction;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpenFolder_triggered();
    void on_actionPause_triggered();
    void on_actionResume_triggered();
    void on_actionPreferences_triggered();
    void on_actionExit_triggered();

    void on_trayIcon_activated(QSystemTrayIcon::ActivationReason reason);
    void on_settingsWidget_logout();

private:
    void createActions();
    void createTrayIcon();
    void createSettingsWidget();

    QSharedPointer<SettingsWidget> settingsWidget;    

    QMenu *trayMenu;
    
    QAction *actionOpenFolder;
    QAction *actionPause;
    QAction *actionResume;
    QAction *actionPreferences;
    QAction *actionExit;


    
};

#endif // MAINWINDOW_H
