#include "RealtimeStyleSheetLoader.h"
#include "QsLog/QsLog.h"

#include <QtCore/QFileSystemWatcher>
#include <QtCore/QFile>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QWidget>

RealtimeStyleSheetLoader::RealtimeStyleSheetLoader(QObject *parent,
                                                   const QString& path)
    : QObject(parent)
    , watcher(new QFileSystemWatcher(this))
    , path(path)
{
    if (path.isEmpty())
        this->path = QCoreApplication::applicationDirPath();

    QLOG_INFO() << "path:" << path;

    watcher->setObjectName("watcher");
    
    QMetaObject::connectSlotsByName(this);
}

bool RealtimeStyleSheetLoader::addWidget(QWidget *widget)
{
    QString watchPath = QString("%1/%2")
        .arg(path)
        .arg(constructFileName(widget));
    
    bool watching = watcher->addPath(watchPath);

    if (watching)
    {
        widgets.insert(watchPath, widget);
        QLOG_INFO() << "Watching:" << watchPath;
    }
    else
    {
        QLOG_INFO() << "Failed to watch:" << watchPath;
    }

    return watching;
}

void RealtimeStyleSheetLoader::on_watcher_fileChanged(const QString &path)
{    
    if (widgets.contains(path))
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        widgets.value(path)->setStyleSheet(file.readAll());
        file.close();
    }
}

QString RealtimeStyleSheetLoader::constructFileName(QWidget *widget)
{
    QString suffix;

#ifdef Q_OS_WIN
    suffix = "win";
#endif

#ifdef Q_OS_MACX
    suffix = "mac";
#endif

#ifdef Q_OS_LINUX
    suffix = "linux";
#endif

    QString classNameNormalized = widget->metaObject()->className();
    classNameNormalized.replace(":", "-");

    return QString("%1-%2.css")
        .arg(classNameNormalized)
        .arg(suffix);

}