#include "factoriesstorage.h"
#include "filesystemfactory.h"
#include "AppController.h"
#include "LoginController.h"

#include "APIClient/ApiTypes.h"
#include "Events/LocalFileEvent.h"
#include "SingleApp/SingleApp.h"
#include "Settings/settings.h"
#include "Util/AppStrings.h"

#include "QsLog/QsLog.h"
#include "QsLog/QsLogDest.h"

#include <QtCore/QTextCodec>
#include <QtCore/QTranslator>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

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

int main(int argc, char *argv[])
{
	SingleApplication app(argc, argv);

	initMetaTypes();
	initTranslator(app);
	initApplicationInfo();
	initLogging();
	logStartInfo();
	initFactories();
	Settings::instance().log();

	if(app.shouldContinue())
	{
		Drive::AppController::instance().setTrayIcon(app.trayIcon());
		Drive::LoginController::instance().showLoginFormOrLogin();
		return app.exec();
	}

	static const auto s_message = QString::fromLatin1(
			"Duplicate instance detected - exiting.");
	QLOG_INFO() << s_message;
	return 0;
}
