#include "factoriesstorage.h"
#include "filesystemfactory.h"
#include "AppController.h"
#include "LoginController.h"
#include "Util/FileUtils.h"

#include "APIClient/ApiTypes.h"
#include "Events/LocalFileEvent.h"
#include "SingleApp/SingleApp.h"
#include "Settings/settings.h"
#include "Util/AppStrings.h"
#include "Tutorial/tutorialplayer.h"
#include "UtilUI/AuxWidgets.h"
#include "FileUtils.h"

#include "QsLog/QsLog.h"
#include "QsLog/QsLogDest.h"

#include <QtCore/QTextCodec>
#include <QtCore/QTranslator>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QException>

#ifdef Q_OS_WIN
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>
#endif

#define LOG_FILE_SIZE 1048576

using namespace Drive;

void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
	static const QString format = QString::fromLatin1("%0 (%1@%2).");
	const QByteArray fullString = format
			.arg(message).arg(context.file).arg(context.line)
			.toLocal8Bit();
	const char* fullMessage = fullString.data();
	switch (type)
	{
		case QtDebugMsg:
			QLOG_DEBUG() << fullMessage;
			break;
		case QtWarningMsg:
			QLOG_WARN() << fullMessage;
			break;
		case QtCriticalMsg:
			QLOG_ERROR() << fullMessage;
			break;
		case QtFatalMsg:
			QLOG_FATAL() << fullMessage;
			break;
	}
}

void initLogging()
{
	using namespace QsLogging;

	Logger& logger = Logger::instance();
	logger.setLoggingLevel(QsLogging::TraceLevel);

	QString logDirPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	QDir dir;
	dir.mkpath(logDirPath);

	const QString sLogPath(QDir(logDirPath).filePath(QString("log.txt")));

	DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(
		sLogPath, EnableLogRotation, MaxSizeBytes(LOG_FILE_SIZE),
		MaxOldLogCount(2)));

	DestinationPtr debugDestination(
		DestinationFactory::MakeDebugOutputDestination());

	logger.addDestination(debugDestination);
	logger.addDestination(fileDestination);

	qInstallMessageHandler(messageHandler);
}

void initMetaTypes()
{
	qRegisterMetaType<Drive::RemoteFileEvent>("RemoteFileEvent");
	qRegisterMetaType<Drive::RemoteFileDesc>("RemoteFileDesc");
	qRegisterMetaType<QList<Drive::RemoteFileDesc> >("RemoteFileDescList");
	qRegisterMetaType<Drive::RemoteFileEventExclusion>("RemoteFileEventExclusion");
	qRegisterMetaType<Drive::LocalFileEvent>("LocalFileEvent");
	qRegisterMetaType<Drive::LocalFileEventExclusion>("LocalFileEventExclusion");
}

void initTranslator(SingleApplication& app)
{
	QTranslator* translator = new QTranslator(&app);
	translator->load(":/drive_ru.qm");
	app.installTranslator(translator);
}

void initApplicationInfo()
{
	QCoreApplication::setOrganizationName(Strings::s_company);
	QCoreApplication::setOrganizationDomain(Strings::s_domain);
	QCoreApplication::setApplicationName(Strings::s_application);
	QCoreApplication::setApplicationVersion(Strings::s_version);
}

void logStartInfo()
{
	static const auto s_message = QString::fromLatin1(
			"New application instance started. Application version: %1."
			" Build timestamp: %2. PID: %3. System encoding: %4.");
	static const auto s_timestamp =
			QString::fromLatin1(__DATE__ " " __TIME__);
	QLOG_INFO() << s_message
			.arg(QCoreApplication::applicationVersion())
			.arg(s_timestamp)
			.arg(QCoreApplication::applicationPid())
			.arg(QString(QTextCodec::codecForLocale()->name()));
}

void initFactories()
{
	FactoriesStorage::instance().registerFileSystem(
			IFileSystemFactoryPtr(new FileSystemFactory()));
}

#ifdef Q_OS_WIN

void killProcessByName(const char *filename)
{
    // QTextStream log("c:\\tmp\\drive_uninstall.log");

    // log << "killProcessByName, file [" << filename << "]" << endl;

    QString filenameStr(strrchr(filename, '\\') + 1);
    DWORD currentProcessId = GetCurrentProcessId();

    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof (pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);

    while (hRes)
    {
        QString szExeFileStr(pEntry.szExeFile);

        // log << "killProcessByName, found process [" << pEntry.th32ProcessID
        //     << ", szExeFile [" << szExeFileStr << "]" << endl;

        // if (strcmp(pEntry.szExeFile, filename) == 0)
        if (filenameStr == szExeFileStr &&
            currentProcessId != pEntry.th32ProcessID)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                                          (DWORD) pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                // log << "Terminating process\n" << endl;
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }

        hRes = Process32Next(hSnapShot, &pEntry);
    }

    // log << "killProcessByName ended" << endl;
    CloseHandle(hSnapShot);
}

#       endif

void deleteAuthorizationData()
{
        // 1. Delete authorization data
        Settings::instance().set(
                    Settings::email, QVariant(QString()), Settings::RealSetting);
        Settings::instance().set(
                    Settings::password, QVariant(QString()), Settings::RealSetting);

        // 2. Prevent force login on next launch
        Settings::instance().set(
                    Settings::autoLogin, QVariant(false), Settings::RealSetting);

        Settings::instance().apply();
}

void doUninstallActions(char *exeName)
{

#       ifdef Q_OS_WIN

        // 0. Kill all app instances on windows
        killProcessByName(exeName);

#       endif

	exeName = 0; // dummy

    deleteAuthorizationData();
    FileSystemHelper::instance().removeLaunchTrackerFile();

#ifdef Q_OS_WIN
    // FolderIconController::unRegisterCOMServer();
#endif
}

void doInstallActions()
{
    deleteAuthorizationData();
    FileSystemHelper::instance().removeLaunchTrackerFile();

#ifdef Q_OS_WIN
    // FolderIconController::registerCOMServer();
#endif
}

int main(int argc, char *argv[])
{

    //
    // Run app
    //

#ifdef Q_OS_DARWIN
    showDockIcon(false);
#endif

    Drive::Utils::setApplicationExePath(argv[0]);
    initApplicationInfo();
    initLogging();
    logStartInfo();

    SingleApplication app(argc, argv);

	initMetaTypes();
	initTranslator(app);
	initFactories();
	Settings::instance().log();


#ifdef Q_OS_WIN

    //
    // Process command line
    //

    try {


    QLOG_DEBUG() << "Begin dump command line arguments" << endl;

    // Command line arguments
    QStringList arguments = app.arguments();
    int argCount = arguments.count();

    // Trace command line arguments
    QLOG_DEBUG() << "Command line arguments count: " << argCount << endl;

    for (int i = 0; i < argCount; i++)
    {
        QLOG_DEBUG() << "argument[" << i << "] = '" << arguments.at(i) << "'" << endl;
    }

    QLOG_DEBUG() << "End dump command line arguments" << endl;

    // If launched for install or uninstall
    if (argCount == 2)
    {
        QString firstArg = arguments.at(1);

        // On install
        if (firstArg == "-install")
        {
            QLOG_TRACE() << "Doing install actions";
            doInstallActions();

            // Exit application
            QLOG_TRACE() << "Exiting application after install actions";
            return 0;
        }

        // On uninstall
        else if (firstArg == "-uninstall")
        {
            // Do actions on application uninstall
            QLOG_TRACE() << "Doing uninstall actions";
            doUninstallActions(argv[0]);

            // Exit application
            QLOG_TRACE() << "Exiting application after uninstall actions";
            return 0;
        }
    }

    } catch (std::exception& ex) {
        QLOG_ERROR() << "Error while accessing command line arguments: [" << ex.what() << "]" << endl;
    } catch (...) {
        QLOG_ERROR() << "Error while accessing command line arguments: " << endl;
    }

#endif // Q_OS_WIN

    //
    // Continue run app
    //

    QLOG_TRACE() << "Main.Continue run app, going to tutorial or login";

    if (app.shouldContinue())
	{
        Drive::AppController::instance().setTrayIcon(app.trayIcon());

        QString diskPath = Settings::instance().get(Settings::folderPath).toString();
        std::string str = diskPath.toStdString();
        QLOG_TRACE() << "Adding folder " << str.c_str() << " to finder favourites";

#ifdef Q_OS_DARWIN
        addToFinderFavorites(str.c_str());
#endif

        QLOG_TRACE() << "Launching...";

        if (1 || FileSystemHelper::instance().isFirstLaunch())
        {
            QLOG_TRACE() << "Launching tutorial";
            emit Drive::AppController::instance().tutorial();
        }
        else
        {
            QLOG_TRACE() << "Launching login";
            emit Drive::AppController::instance().login();
        }

        int retCode = app.exec();

        while (retCode == 0)
        {
            retCode = app.exec();
        }

        QLOG_TRACE() << "Exiting event loop, retCode = " << retCode;

        return retCode;
	}

	static const auto s_message = QString::fromLatin1(
			"Duplicate instance detected - exiting.");

	QLOG_INFO() << s_message;

	return 0;
}
