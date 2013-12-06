#include "FileUtils.h"
#include "AppStrings.h"
#include "QsLog/QsLog.h"

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QCoreApplication>

#ifdef Q_OS_WIN
    #include "windows.h"
#endif // Q_OS_WIN

namespace Drive
{

FileSystemHelper& FileSystemHelper::instance()
{
    static FileSystemHelper myself;
    return myself;
}

FileSystemHelper::FileSystemHelper(QObject *parent)
    : QObject(parent)
{
}

void FileSystemHelper::setWindowsFolderIcon(const QString& folderPath,
                                            int iconNumber)
{

#ifdef Q_OS_WIN

    QFileInfo fileInfo(folderPath);

    if (!fileInfo.exists())
        return;

    if (!fileInfo.isDir())
        return;
    
    QFile desktopIni(folderPath + "/desktop.ini");
    if (desktopIni.exists())
    {
        if (!desktopIni.remove())
        {
            QLOG_ERROR() << "Can't remove file: " << desktopIni.fileName();
            return;
        }
    }

    if (!desktopIni.open(QIODevice::Text | QIODevice::WriteOnly))
        return;

    QString content = QString(tr(
        "[.ShellClassInfo]" 
        "InfoTip=%1\n"
        "IconFile=%2\n"
        "IconIndex=%3"))
        .arg(QString(tr("%1 Folder")).arg(Strings::appFullName))
        .arg(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()))
        .arg(iconNumber);

    desktopIni.write(content.toLocal8Bit());
    desktopIni.close();

    SetFileAttributes(QDir::toNativeSeparators(desktopIni.fileName()).toStdString().c_str(),
        FILE_ATTRIBUTE_HIDDEN);

    QFile::Permissions perm;
    //perm = QFile::permissions(folderPath);
    perm = QFile::ReadOwner | QFile::WriteOwner;

    bool isOK = QFile::setPermissions(folderPath, perm);

    perm = QFile::ReadOwner;
    isOK = QFile::setPermissions(folderPath, perm);

#endif // Q_OS_WIN

}

}