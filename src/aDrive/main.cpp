#include "SingleApp/SingleApp.h"
#include "QsLog/QsLog.h"
#include "QsLog/QsLogDest.h"
#include "mainwindow.h"

#include <QtCore/QDir>

#define LOG_FILE_SIZE 1048576

int main(int argc, char *argv[])
{
	
    
    SingleApplication app(argc, argv);

	if(app.shouldContinue())
	{
        QCoreApplication::setOrganizationName("Assistent");
        QCoreApplication::setOrganizationDomain("www.assistent.by");
        QCoreApplication::setApplicationName("Drive");
        QCoreApplication::setApplicationVersion("1.0a");        
        
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

        QLOG_INFO() << "Started";
        
        MainWindow w;
		//w.show();
        return app.exec();
	}

	return 0;
}
