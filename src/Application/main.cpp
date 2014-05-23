#include "AppController.h"
#include "LoginController.h"

#include "Util/AppStrings.h"
#include "SingleApp/SingleApp.h"
#include "QsLog/QsLog.h"
#include "QsLog/QsLogDest.h"

#include "APIClient/ApiTypes.h"
#include "Events/LocalFileEvent.h"

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

	QLOG_TRACE() << "Started (" << QCoreApplication::applicationPid() << ").";
}

int main(int argc, char *argv[])
{
	SingleApplication app(argc, argv);

	qRegisterMetaType<Drive::RemoteFileEvent>("RemoteFileEvent");
	qRegisterMetaType<Drive::RemoteFileDesc>("RemoteFileDesc");
	qRegisterMetaType<QList<Drive::RemoteFileDesc> >("RemoteFileDescList");
	qRegisterMetaType<Drive::RemoteFileEventExclusion>("RemoteFileEventExclusion");
	qRegisterMetaType<Drive::LocalFileEvent>("LocalFileEvent");
	qRegisterMetaType<Drive::LocalFileEventExclusion>("LocalFileEventExclusion");

	QTranslator translator;
	translator.load(":/drive_ru.qm");
	app.installTranslator(&translator);

	QCoreApplication::setOrganizationName(Strings::s_company);
	QCoreApplication::setOrganizationDomain(Strings::s_domain);
	QCoreApplication::setApplicationName(Strings::s_application);
	QCoreApplication::setApplicationVersion(Strings::s_version);

	initLogging();
	qInstallMessageHandler(messageHandler);
	QLOG_TRACE() << "Should continue: " << app.shouldContinue();

	if(app.shouldContinue())
	{
		Drive::AppController::instance().setTrayIcon(app.trayIcon());
		Drive::LoginController::instance().showLoginFormOrLogin();
		return app.exec();
	}

	return 0;
}
