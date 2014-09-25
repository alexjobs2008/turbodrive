#include "FileUtils.h"
#include "AppStrings.h"
#include "QsLog/QsLog.h"
#include "Settings/settings.h"
#include "Events/LocalFileEventNotifier.h"

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QtGui/QIcon>

#include <list>

#define DISK_ROOT_PATH QLatin1String("#root/#disk")

using namespace std;

namespace Drive
{

FileSystemHelper& FileSystemHelper::instance()
{
	static FileSystemHelper myself;
	return myself;
}

FileSystemHelper::FileSystemHelper(QObject *parent)
    : QObject(parent),
      m_isFirstLaunch(true)
{
    trackLaunches();
}

void FileSystemHelper::trackLaunches()
{
    QString logDirPath = QStandardPaths::writableLocation(
                QStandardPaths::DataLocation);

    QString fileName(QDir::cleanPath(logDirPath).append("/").append(".MTSDrive.launchTracker"));

    if (QFile::exists(fileName))
    {
        m_isFirstLaunch = false;
    }
    else
    {
        QFile file(fileName);
        file.open(QIODevice::Append);
        file.write("");
        file.close();
    }
}

void FileSystemHelper::setFolderIcon(
		const QString& folderPath, int iconNumber)
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
		.arg(QString(tr("%1 Folder"))
			.arg(Strings::getAppString(Strings::AppFullName)))
		.arg(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()))
		.arg(iconNumber);

	desktopIni.write(content.toLocal8Bit());
	desktopIni.close();

	SetFileAttributes(
		(LPCWSTR)QDir::toNativeSeparators(desktopIni.fileName()).utf16()
		, FILE_ATTRIBUTE_HIDDEN);

	QFile::setPermissions(folderPath, QFile::ReadOwner);

#elif defined(Q_OS_DARWIN)

    std::string folderPathStr = folderPath.toStdString();
    const char *folderStr = folderPathStr.c_str();

    /* // Default icon
    const char *macIconResource = "";

    // Select icon
    if (iconNumber == FOLDER_ICON_OK) macIconResource = ":/folder_icons/mac/512_ok.png";
    else if (iconNumber == FOLDER_ICON_ERROR) macIconResource = ":/folder_icons/mac/512_error.png";
    else if (iconNumber == FOLDER_ICON_SYNC) macIconResource = ":/folder_icons/mac/128_sync.png";

    QIcon icon;

    icon.addPixmap(QPixmap(macIconResource));
    QSize size = icon.actualSize(QSize(128000, 128000));
    QPixmap pixmap = icon.pixmap(size);
    QByteArray ba;              // Construct a QByteArray object
    QBuffer buffer(&ba);        // Construct a QBuffer object using the QbyteArray
    QImage image = pixmap.toImage();
    image.save(&buffer, "PNG");

    setFolderIconFromQIcon(folderStr, ba.data(), ba.size()); */

    QString iconPath = Utils::parentPath(Utils::parentPath(
         Utils::getApplicationExePath())) + "/Resources/";

    if (iconNumber == FOLDER_ICON_OK) iconPath += "ok.icns";
    else if (iconNumber == FOLDER_ICON_ERROR) iconPath += "error.icns";
    else if (iconNumber == FOLDER_ICON_SYNC) iconPath += "sync.icns";

    setFolderIconFromPath(folderStr, iconPath.toStdString().c_str());

#else
	(void)folderPath;
	(void)iconNumber;
#endif // Q_OS_WIN
}

bool FileSystemHelper::setFileModificationTimestamp(const QString& localPath, uint modifiedAt)
{
#ifdef Q_OS_WIN
    std::wstring filePathWStr = localPath.toStdWString();
    LPCWSTR filePath = (LPCWSTR)
            // QDir::toNativeSeparators(localPath).utf16();
            filePathWStr.c_str();

	HANDLE hFile = CreateFile(filePath, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	FILETIME ft = FileSystemHelper::toWinFileTime(QDateTime::fromTime_t(modifiedAt));

	BOOL result = SetFileTime(hFile, (LPFILETIME) NULL, (LPFILETIME) NULL, &ft);
	QLOG_TRACE() << "File modification time set:" << result;
	CloseHandle(hFile);
	return result;
#else
	(void)localPath;
	(void)modifiedAt;
	return true;
#endif
}

bool FileSystemHelper::removeDirWithSubdirs(const QString &dirName,
											bool notifyLocalWatcher)
{
	bool result = true;
	QDir dir(dirName);

	if (dir.exists(dirName))
	{
		Q_FOREACH(QFileInfo info,
			dir.entryInfoList(QDir::NoDotAndDotDot
							| QDir::System
							| QDir::Hidden
							| QDir::AllDirs
							| QDir::Files, QDir::DirsFirst))
		{
			if (notifyLocalWatcher)
			{
//				LocalFileEventNotifier::instance()
//					.addExclusion(info.absoluteFilePath());
			}

			if (info.isDir())
			{
				result = removeDirWithSubdirs(info.absoluteFilePath());
			}
			else
			{
				result = QFile::remove(info.absoluteFilePath());
			}

			if (!result)
				return result;
		}
		result = dir.rmdir(dirName);
	}

	return result;
}

list<QFileInfo> FileSystemHelper::entries(const string& dir)
{
	list<QFileInfo> result;
	QDir qDir(QString::fromStdString(dir));
	Q_ASSERT(qDir.exists());
	for(const auto info: qDir.entryInfoList(
			QDir::NoDotAndDotDot
			| QDir::System
			| QDir::AllDirs
			| QDir::Files, QDir::DirsFirst))
	{
		result.push_back(info);
	}
	return result;
}

#ifdef Q_OS_WIN
FILETIME FileSystemHelper::toWinFileTime(const QDateTime &dateTime)
{
	// Definition of FILETIME from MSDN:
	// Contains a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC).
	QDateTime origin(QDate(1601, 1, 1), QTime(0, 0, 0, 0), Qt::UTC);
	// Get offset - note we need 100-nanosecond intervals, hence we multiply by
	// 10000.
	qint64 _100nanosecs = 10000 * origin.msecsTo(dateTime);
	// Pack _100nanosecs into the structure.
	FILETIME fileTime;
	fileTime.dwLowDateTime = _100nanosecs;
	fileTime.dwHighDateTime = (_100nanosecs >> 32);
    return fileTime;
}
#endif // Q_OS_WIN

bool FileSystemHelper::isFirstLaunch()
{
    return m_isFirstLaunch;
}

QString Utils::separator()
{
	return QLatin1String("/");
}

QString Utils::parentPath(const QString &path)
{
	return QDir::cleanPath(path + separator() + QLatin1String(".."));
}

QString Utils::toLocalPath(const QString& remotePath)
{
	const QString folderPath = QDir::cleanPath(
		Settings::instance().get(Settings::folderPath).toString());

	QString cleanedRemotePath = QDir::cleanPath(remotePath);

	Q_ASSERT(cleanedRemotePath.startsWith(DISK_ROOT_PATH));

	return cleanedRemotePath.replace(DISK_ROOT_PATH, folderPath);
}

QString Utils::toRemotePath(const QString& localPath)
{
	const QString folderPath = QDir::cleanPath(
		Settings::instance().get(Settings::folderPath).toString());

	QString cleanedLocalPath = QDir::cleanPath(localPath);

    // Q_ASSERT(cleanedLocalPath.startsWith(folderPath));
    bool startsWithFolderPath = cleanedLocalPath.startsWith(folderPath);
    if (!startsWithFolderPath)
    {
        QLOG_ERROR() << "Utils::toRemotePath() cleanedLocalPath.startsWith(folderPath) == false, "
                        << "cleanedLocalPath = [" << cleanedLocalPath << "], "
                           << "= [" << folderPath << "]";
        return QString();
    }

	return cleanedLocalPath.replace(folderPath, DISK_ROOT_PATH);
}

static QString applicationExePath;

void Utils::setApplicationExePath(char *path)
{
    applicationExePath = QString(path);
}

QString& Utils::getApplicationExePath()
{
    return applicationExePath;
}

}
