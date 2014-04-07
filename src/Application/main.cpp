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
	// TODO: remove this and use toUTF8 and fromUTF8 + QLatin1String
	// http://qt-project.org/wiki/Strings_and_encodings_in_Qt
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

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

	QCoreApplication::setOrganizationName(Strings::companyNameEn);
	QCoreApplication::setOrganizationDomain(Strings::companyDomain);
	QCoreApplication::setApplicationName(Strings::appNameEn);
	QCoreApplication::setApplicationVersion(Strings::appVersion);

	initLogging();
	QLOG_TRACE() << "Should continue: " << app.shouldContinue();

	if(app.shouldContinue())
	{
		Drive::AppController::instance().setTrayIcon(app.trayIcon());
		Drive::LoginController::instance().showLoginFormOrLogin();
		return app.exec();
	}

	return 0;
}
