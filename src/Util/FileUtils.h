#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <QtCore/QObject>
#include <QtCore/QFileInfo>

#ifdef Q_OS_WIN
	#include "Windows.h"
#endif

namespace Drive
{

class FileSystemHelper : public QObject
{
	Q_OBJECT
public:
	static FileSystemHelper& instance();

	static void setWindowsFolderIcon(const QString& folderPath,
									int iconNumber);

	static bool setFileModificationTimestamp(const QString& localPath,
											uint modifiedAt);

	static bool removeDirWithSubdirs(const QString &dirName,
									bool notifyLocalWatcher = true);

	static std::list<QFileInfo> entries(const std::string& dir);

#ifdef Q_OS_WIN
	static FILETIME toWinFileTime(const QDateTime &dateTime);
#endif

    bool isFirstLaunch();

private:
	Q_DISABLE_COPY(FileSystemHelper)
	explicit FileSystemHelper(QObject *parent = 0);
    void trackLaunches();

    bool m_isFirstLaunch;
};

class Utils
{
public:
	static QString separator();
	static QString parentPath(const QString& path);
	static QString toLocalPath(const QString& remotePath);
	static QString toRemotePath(const QString& localPath);
};

}


#endif // FILE_UTILS_H
