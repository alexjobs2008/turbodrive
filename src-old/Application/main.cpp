#include "AppController.h"

#include "Util/AppStrings.h"
#include "SingleApp/SingleApp.h"
#include "QsLog/QsLog.h"
#include "QsLog/QsLogDest.h"

#include <QtCore/QDir>

#define LOG_FILE_SIZE 1048576

using namespace Drive;

int main(int argc, char *argv[])
{    
    SingleApplication app(argc, argv);

	if(app.shouldContinue())
	{
        QCoreApplication::setOrganizationName(Strings::companyNameEn);
        QCoreApplication::setOrganizationDomain(Strings::companyDomain);
        QCoreApplication::setApplicationName(Strings::appNameEn);
        QCoreApplication::setApplicationVersion(Strings::appVersion);
        
        using namespace QsLogging;

        // init the logging mechanism
        Logger& logger = Logger::instance();
        logger.setLoggingLevel(QsLogging::TraceLevel);
        const QString
            sLogPath(QDir(app.applicationDirPath()).filePath("log.txt"));

        DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(
            sLogPath, EnableLogRotation, MaxSizeBytes(LOG_FILE_SIZE), MaxOldLogCount(2)));

        DestinationPtr debugDestination(
            DestinationFactory::MakeDebugOutputDestination());
        
        logger.addDestination(debugDestination);
        logger.addDestination(fileDestination);

        QLOG_TRACE() << "Started";
        
        Drive::AppController::instance();
        return app.exec();
	}

	return 0;
}
