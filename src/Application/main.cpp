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

#include "QsLog/QsLog.h"
#include "QsLog/QsLogDest.h"

#include <QtCore/QTextCodec>
#include <QtCore/QTranslator>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

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

void doUninstallActions(char *exeName)
{

#       ifdef Q_OS_WIN

        // 0. Kill all app instances on windows
        killProcessByName(exeName);

#       endif

	exeName = 0; // dummy

        // 1. Delete authorization data
        Settings::instance().set(
                    Settings::email, QVariant(QString()), Settings::RealSetting);
        Settings::instance().set(
                    Settings::password, QVariant(QString()), Settings::RealSetting);

        // 2. Prevent force login on next launch
        Settings::instance().set(
                    Settings::autoLogin, QVariant(false), Settings::RealSetting);

}

int main(int argc, char *argv[])
{
    //
    // Process command line
    //

   /*  QLOG_DEBUG() << "Command line argc: " << argc << endl;
    for (int i = 0; i < argc; i++)
        QLOG_DEBUG() << "argv[" << i << "] = '" << argv[i] << "'" << endl; */

    // If launched during uninstall
    if (argc == 2 && strcmp(argv[1], "-uninstall") == 0)
    {
        // Do actions on application uninstall
        doUninstallActions(argv[0]);

        // Exit application
        return 0;
    }

    //
    // Run app
    //

    initApplicationInfo();
    initLogging();
    logStartInfo();

    SingleApplication app(argc, argv);

	initMetaTypes();
	initTranslator(app);
	initFactories();
	Settings::instance().log();

	if(app.shouldContinue())
	{
        Drive::AppController::instance().setTrayIcon(app.trayIcon());

        if (FileSystemHelper::instance().isFirstLaunch())
        {
            emit Drive::AppController::instance().tutorial();
        }
        else
        {
            emit Drive::AppController::instance().login();
        }

        int retCode = app.exec();
        return retCode;
	}

	static const auto s_message = QString::fromLatin1(
			"Duplicate instance detected - exiting.");
	QLOG_INFO() << s_message;
	return 0;
}
