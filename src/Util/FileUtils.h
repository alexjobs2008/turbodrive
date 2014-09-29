#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <QtCore/QObject>
#include <QtCore/QFileInfo>
#include <QHash>
#include <QString>
#include <QBuffer>

#ifdef Q_OS_WIN
#include "Windows.h"
#endif


namespace Drive
{


// No state set for file or folder
const int FOLDER_STATE_NOT_SET = 0;

// Corresponds to folder or file state
const int FOLDER_ICON_OK = 1;
const int FOLDER_ICON_ERROR = 2;
const int FOLDER_ICON_SYNC = 3;

const int FOLDER_STATE_DELETED = 100;

class FileSystemHelper : public QObject
{
	Q_OBJECT
public:
	static FileSystemHelper& instance();

    static void setFolderIcon(const QString& folderPath,
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

public:
    void removeLaunchTrackerFile();

private:
    bool m_isFirstLaunch;
};

class Utils
{
public:
	static QString separator();
	static QString parentPath(const QString& path);
	static QString toLocalPath(const QString& remotePath);
	static QString toRemotePath(const QString& localPath);
    static void setApplicationExePath(char *path);
    static QString& getApplicationExePath();
};


//
// This singleton object sets file or folder synchronization state
//
class FolderIconController : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(FolderIconController)

    // Map file full path to state
    QHash<QString, int> statesMap;

    // Disable constructor
    explicit FolderIconController(QObject *parent = 0);

public:
    static FolderIconController& instance();
    static void registerCOMServer();
    static void unRegisterCOMServer();

private slots:

    //
    // State actions handlers
    //

    void handleSetState(QString &fileName, int state);
    void handleSetDeleted(QString &fileName);

signals:

    //
    // State action signals
    //

    // Record file state
    // state is one of: FOLDER_ICON_OK _ERROR _SYNC
    void setStateSignal(QString& fileName, int state);

    // Mark file deleted and remove it from map
    void setDeletedSignal(QString& fileName);

public :

    // Record file state
    // state is one of: FOLDER_ICON_OK _ERROR _SYNC
    void setState(QString& fileName, const int state);

    // Mark file deleted and remove it from map
    void setDeleted(QString& fileName);

    // Get recorded file state
    int getState(QString& fileName);

}; // class FolderIconController


#ifdef Q_OS_WIN

void setWinStateAttribute(QString& fileName, int state);

#endif


//
// Mac OS X functions
//

#ifdef Q_OS_DARWIN

struct CGImage;
typedef struct CGImage *CGImageRef;

extern "C" {

// Routine for setting file or folder icon badge on Mac OS
// Second parameter used for not to read it from map
void setBadge(QString& fileName, int state);

// Folder icons Mac operations
bool setFolderIconFromPath(const char *folderURL, const char *iconPath);
bool setFolderIconFromQIcon(const char *folderURL, char *imageBytes, int imageSize);

// Add folder to favorites
bool addToFinderFavorites(const char *folder);

}

#endif // Q_OS_DARWIN


} // namespace Drive


#endif // FILE_UTILS_H
