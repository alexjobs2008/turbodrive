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
#include <QtCore/QThread>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QtGui/QIcon>

#include <list>

#define DISK_ROOT_PATH QLatin1String("#root/#disk")

using namespace std;

namespace Drive
{


//
// FileSystemHelper
//

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

void FileSystemHelper::removeLaunchTrackerFile()
{
    QString logDirPath = QStandardPaths::writableLocation(
                QStandardPaths::DataLocation);

    QString fileName(QDir::cleanPath(logDirPath).append("/").append(".MTSDrive.launchTracker"));

    QFile::remove(fileName);
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

    /* std::string folderPathStr = folderPath.toStdString();
    const char *folderStr = folderPathStr.c_str();

    // Default icon
    const char *macIconResource = "";

    // Select icon
    if (iconNumber == FOLDER_ICON_OK) macIconResource = ":/folder_icons/mac/512_ok.png";
    else if (iconNumber == FOLDER_ICON_ERROR) macIconResource = ":/folder_icons/mac/512_error.png";
    else if (iconNumber == FOLDER_ICON_SYNC) macIconResource = ":/folder_icons/mac/128_sync.png";

    if (iconNumber == Drive::FOLDER_ICON_OK) macIconResource = ":/folder_icons/ok.icns";
    else if (iconNumber == Drive::FOLDER_ICON_ERROR) macIconResource = ":/folder_icons/error.icns";
    else if (iconNumber == Drive::FOLDER_ICON_SYNC) macIconResource = ":/folder_icons/sync.icns";

    QIcon icon;

    icon.addPixmap(QPixmap(macIconResource));
    QSize size = icon.actualSize(QSize(128000, 128000));
    QPixmap pixmap = icon.pixmap(size);
    QByteArray ba;              // Construct a QByteArray object
    QBuffer buffer(&ba);        // Construct a QBuffer object using the QbyteArray
    QImage image = pixmap.toImage();
    image.save(&buffer, "PNG"); */

    // setFolderIconFromQIcon(folderStr, ba.data(), ba.size());

    /* QString iconPath = Utils::parentPath(Utils::parentPath(
         Utils::getApplicationExePath())) + "/Resources/";

    if (iconNumber == FOLDER_ICON_OK) iconPath += "ok.icns";
    else if (iconNumber == FOLDER_ICON_ERROR) iconPath += "error.icns";
    else if (iconNumber == FOLDER_ICON_SYNC) iconPath += "sync.icns";

    setFolderIconFromPath(folderStr, iconPath.toStdString().c_str()); */

    (void)folderPath;
    (void)iconNumber;
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



//
// Utils
//

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


//
// FolderIconController
//

FolderIconController &FolderIconController::instance()
{
    static FolderIconController controller;
    return controller;
}

#ifdef Q_OS_WIN

void FolderIconController::registerCOMServer()
{
    HMODULE dll = LoadLibrary(L"libCOM.dll");
    typedef HRESULT(__stdcall *DllRegisterServerProc)();
    DllRegisterServerProc DllRegisterServer =
        (DllRegisterServerProc)GetProcAddress(dll, "DllRegisterServer");

    HRESULT res = E_FAIL;
    if (DllRegisterServer)
    {
        res = DllRegisterServer();
    }

    if (res != S_OK)
    {
        printf("DllRegisterServer for libCOM.dll failed with error code: %x",
                (int) GetLastError());
        QLOG_ERROR() << "DllRegisterServer for libCOM.dll failed with error code: "
                        << GetLastError();
    }
}

void FolderIconController::unRegisterCOMServer()
{
    HMODULE dll = LoadLibrary(L"libCOM.dll");
    typedef HRESULT(__stdcall *DllUnRegisterServerProc)();
    DllUnRegisterServerProc DllUnregisterServer =
        (DllUnRegisterServerProc)GetProcAddress(dll, "DllUnregisterServer");

    HRESULT res = E_FAIL;
    if (DllUnregisterServer)
    {
        res = DllUnregisterServer();
    }

    if (res != S_OK)
    {
        QLOG_ERROR() << "DllRegisterServer for libCOM.dll failed with error code: "
                        << GetLastError();
    }
}

#endif

//
// FolderIconController constructor
// Connect signals
//
FolderIconController::FolderIconController(QObject *parent) :
    QObject(parent)
{
    connect(this, &FolderIconController::setStateSignal,
            this, &FolderIconController::handleSetState,
            Qt::QueuedConnection);

    connect(this, &FolderIconController::setDeletedSignal,
            this, &FolderIconController::handleSetDeleted,
            Qt::QueuedConnection);
}

//
// State actions handlers
//

void FolderIconController::handleSetState(QString &fileName, int state)
{
    //
    // 1 Serialize
    //
    QMutexLocker locker(&mutex);

    //
    // Set state
    //
    stateMap[fileName] = state;

    //
    // Set syncronization events counter for one folder/file
    //
    QHash<QString, int>::iterator counterIter = stateCounter.find(fileName);

    if (counterIter != stateCounter.end())
    {
        counterIter.value()++;
    }
    else
    {
        stateCounter[fileName] = 1;
    }

    /*
    //
    // Set file/folder mark (icon)
    //
#ifdef Q_OS_DARWIN
    setBadge(fileName, state);
#endif
#ifdef Q_OS_WIN
    QFileInfo fileInfo(fileName);

    if (fileInfo.isDir())
    {
        // FileSystemHelper::setFolderIcon(fileName, state);
        setWinStateAttribute(fileName, state);
    }
    else
    {
        setWinStateAttribute(fileName, state);
    }
#endif
    */

}

void FolderIconController::handleSetDeleted(QString &fileName)
{
    QMutexLocker locker(&mutex);
    stateMap.remove(fileName);
}

//
// State actions signal emitters
//

void FolderIconController::setState(QString &fileName, const int state)
{
    // emit setStateSignal(fileName, state);
    handleSetState(fileName, state);
    // FileSystemHelper::setFolderIcon(fileName, state);
}

void FolderIconController::setDeleted(QString &fileName)
{
    // emit setDeletedSignal(fileName);
    handleSetDeleted(fileName);
}

int FolderIconController::getState(QString &fileName)
{
    QMutexLocker locker(&mutex);

    QHash<QString, int>::iterator iter = stateMap.find(fileName);

    // Found
    if (iter != stateMap.end())
    {
        // Get current state
        int state = iter.value();

        if (state == FOLDER_STATE_FINISHED)
        {
            stateMap[fileName] = FOLDER_STATE_NOT_SET;
        }

        return state;
    }

    // Not found
    else
    {
        return FOLDER_STATE_NOT_SET;
    }

    return FOLDER_STATE_NOT_SET;
}

// Return true if all 4 sync events occured, false otherwise
bool FolderIconController::getCounter(QString &fileName)
{
    QMutexLocker locker(&mutex);

    QHash<QString, int>::iterator counterIter = stateCounter.find(fileName);

    if (counterIter != stateCounter.end())
    {
        counterIter.value()++;

        // If all sync events occured, reset counter
        if (counterIter.value() > 4)
        {
            counterIter.value() = 0;
            return true;
        }
    }
    else
    {
        return false;
    }

    return false;
}

void FolderIconController::resetAllCounters()
{
    QMutexLocker locker(&mutex);
    stateCounter.clear();
}

#ifdef Q_OS_WIN

void setWinStateAttribute(QString fileName, int state)
{
    // If file was deleted then do not set attributes
    if (!QFile::exists(fileName)) return;

    // Stream name
    QString fileNameStr(fileName);
    std::wstring orgFileNameW = fileNameStr.toStdWString();

    // Read file times
    HANDLE fileHandle = CreateFile(orgFileNameW.c_str(),
        GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    FILETIME lpCreationTime = {0, 0};
    FILETIME lpLastAccessTime = {0, 0};
    FILETIME lpLastWriteTime = {0, 0};

    BOOL fileResult = GetFileTime(fileHandle, &lpCreationTime,
      &lpLastAccessTime, &lpLastWriteTime);

    CloseHandle(fileHandle);

    // Create file stream name
    fileNameStr += ":Stream:$DATA";
    std::wstring fileNameW = fileNameStr.toStdWString();

    // Open stream for exclusive read
    HANDLE hStream = CreateFile(
        fileNameW.c_str(),            // Filename
        GENERIC_WRITE,           // Desired access
        FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share flags
        NULL,                    // Security Attributes
        CREATE_ALWAYS,           // Creation Disposition
        FILE_ATTRIBUTE_NORMAL,                        // Flags and Attributes
        NULL);                   // OVERLAPPED pointer

    // If open error
    if (hStream == INVALID_HANDLE_VALUE)
    {
        QLOG_ERROR() << "Could not open stream for file [" << fileNameStr <<
                       "] GetlastError() = " << GetLastError();
    }

    // If ok rewrite state
    else
    {
        char buf[1] = {0};
        buf[0] = state;
        DWORD numberOfBytesWritten = 0;

        BOOL writeResult = WriteFile(
            hStream,            // _In_         HANDLE hFile,
            buf,                // _In_         LPCVOID lpBuffer,
            1,                  // _In_         DWORD nNumberOfBytesToWrite,
            &numberOfBytesWritten,               // _Out_opt_    LPDWORD lpNumberOfBytesWritten,
            NULL                // _Inout_opt_  LPOVERLAPPED lpOverlapped
        );

        if (!writeResult)
        {
           QLOG_ERROR() << "Could not write to stream for file [" << fileNameStr <<
                           "] GetlastError() = " << GetLastError();
        }

        CloseHandle(hStream);

        // Restore file times
        HANDLE fileHandle = CreateFile(orgFileNameW.c_str(),
            GENERIC_READ | FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, 0, NULL);

        fileResult = SetFileTime(fileHandle, &lpCreationTime,
          &lpLastAccessTime, &lpLastWriteTime);

        (void) fileResult;
        CloseHandle(fileHandle);
    }
}

#endif

} // namespace Drive

#ifdef Q_OS_DARWIN

void setBadge(QString& fileName, int state)
{
    std::string folderPathStr = fileName.toStdString();
    const char *folderStr = folderPathStr.c_str();

    // Default icon
    const char *macIconResource = "";

    // Select icon
    if (state == Drive::FOLDER_ICON_OK) macIconResource = ":/folder_icons/mac/512_ok.png";
    else if (state == Drive::FOLDER_ICON_ERROR) macIconResource = ":/folder_icons/mac/512_error.png";
    else if (state == Drive::FOLDER_ICON_SYNC) macIconResource = ":/folder_icons/mac/512_sync.png";

    /* if (state == Drive::FOLDER_ICON_OK) macIconResource = ":/folder_icons/ok.icns";
    else if (state == Drive::FOLDER_ICON_ERROR) macIconResource = ":/folder_icons/error.icns";
    else if (state == Drive::FOLDER_ICON_SYNC) macIconResource = ":/folder_icons/sync.icns"; */

    QIcon icon;

    icon.addPixmap(QPixmap(macIconResource));
    QSize size = icon.actualSize(QSize(512, 512));
    QPixmap pixmap = icon.pixmap(size);
    QByteArray ba;              // Construct a QByteArray object
    QBuffer buffer(&ba);        // Construct a QBuffer object using the QbyteArray
    QImage image = pixmap.toImage();
    image.save(&buffer, "PNG");

    QString homePath = Drive::Settings::instance().get(Drive::Settings::folderPath).toString();
    char *fImageBytes = 0; int fImageSize = 0;

    QIcon icon2;
    QByteArray ba2;              // Construct a QByteArray object
    QBuffer buffer2(&ba2);        // Construct a QBuffer object using the QbyteArray

    if (fileName == homePath)
    {
        icon2.addPixmap(QPixmap(":/folder_icons/mac/folder-mac-512.png"));
        QSize size2 = icon2.actualSize(QSize(512, 512));
        QPixmap pixmap2 = icon2.pixmap(size2);
        QImage image2 = pixmap2.toImage();
        image2.save(&buffer2, "PNG");
        fImageBytes = ba2.data(); fImageSize = ba2.size();
    }

    bool didSetIcon = setBadgeIcon2(folderStr, ba.data(), ba.size(), fImageBytes, fImageSize);
    // bool didSetIcon = setBadgeIcon(folderStr, ba.data(), ba.size());
    QLOG_TRACE() << "setBadgeIcon2 for file [" << fileName << "] to [" << macIconResource << "] returned [" << didSetIcon <<  "]";
}

#endif

