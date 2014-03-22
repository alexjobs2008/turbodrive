#include "FileUtils.h"
#include "AppStrings.h"
#include "QsLog/QsLog.h"
#include "Settings/settings.h"
//#include "Events/LocalFileEvent.h"
#include "Events/LocalFileEventNotifier.h"

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>

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
		.arg(QString(tr("%1 Folder"))
			.arg(Strings::getAppString(Strings::AppFullName)))
		.arg(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()))
		.arg(iconNumber);

	desktopIni.write(content.toLocal8Bit());
	desktopIni.close();

	SetFileAttributes(
		(LPCWSTR)QDir::toNativeSeparators(desktopIni.fileName()).utf16()
		, FILE_ATTRIBUTE_HIDDEN);

	QFile::Permissions perm;
	//perm = QFile::permissions(folderPath);
	perm = QFile::ReadOwner | QFile::WriteOwner;

	bool isOK = QFile::setPermissions(folderPath, perm);

	perm = QFile::ReadOwner;
	isOK = QFile::setPermissions(folderPath, perm);

#endif // Q_OS_WIN
}

bool FileSystemHelper::setFileModificationTimestamp(const QString& localPath, uint modifiedAt)
{
#ifdef Q_OS_WIN
	LPCWSTR filePath = (LPCWSTR) QDir::toNativeSeparators(localPath).utf16();

	HANDLE hFile = CreateFile(filePath, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	FILETIME ft = FileSystemHelper::toWinFileTime(QDateTime::fromTime_t(modifiedAt));

	BOOL result = SetFileTime(hFile, (LPFILETIME) NULL, (LPFILETIME) NULL, &ft);
	QLOG_TRACE() << "File modification time set:" << result;
	CloseHandle(hFile);
	return result;
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

Utils& Utils::instance()
{
	static Utils myself;
	return myself;
}

Utils::Utils(QObject *parent)
	: QObject(parent)
{
}

QString Utils::remotePathToLocalPath(const QString& remotePath, bool addSeparapor)
{
	QString folderPath = QDir::toNativeSeparators(
		Settings::instance().get(Settings::folderPath).toString());
	QString localPath = remotePath;

	if (localPath.startsWith("/") || localPath.startsWith("\\"))
	{
		localPath = localPath.right(localPath.size() - 1);
	}

	if (addSeparapor)
		if (!localPath.endsWith("/") && !localPath.endsWith("\\"))
			localPath.append(QDir::separator());

	if (localPath.contains(DISK_ROOT_PATH))
	{
		localPath.replace(DISK_ROOT_PATH, folderPath);
	}
	else
	{
		QString anotherSeparatorsRootDisk =
			QDir::toNativeSeparators(DISK_ROOT_PATH);

		if (localPath.contains(anotherSeparatorsRootDisk))
		{
			localPath.replace(anotherSeparatorsRootDisk, folderPath);
		}
		else
		{
			QLOG_TRACE() << "Util::remotePathToLocalPath(): bad remotePath:"
				<< remotePath;
		}
	}

	return QDir::toNativeSeparators(localPath);
}

QString Utils::localPathToRemotePath(const QString& localPath)
{
	QString folderPath = QDir::toNativeSeparators(
		Settings::instance().get(Settings::folderPath).toString());
	QString remotePath = QDir::toNativeSeparators(localPath);

	remotePath.replace(folderPath, DISK_ROOT_PATH);

	return QDir::fromNativeSeparators(remotePath);
}

}