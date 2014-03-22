#ifndef REALTIME_STYLESHEET_LOADER
#define REALTIME_STYLESHEET_LOADER

#include <QtCore/QObject>
#include <QtCore/QMap>

class QFileSystemWatcher;
class QWidget;

class RealtimeStyleSheetLoader : public QObject
{
    Q_OBJECT
public:
    RealtimeStyleSheetLoader(QObject *parent = 0,
        const QString& path = QString());

    bool addWidget(QWidget *widget);

private slots:
    void on_watcher_fileChanged(const QString &path);

private:
    static QString constructFileName(QWidget *widget);

    QFileSystemWatcher* watcher;
    QString path;

    QMap<QString, QWidget*> widgets;
};

#endif REALTIME_STYLESHEET_LOADER