#include "EventHandlers.h"

namespace Drive
{

FileEvent::FileEvent(const QString& json)
{

}

FileEvent::FileEvent(EventType type, const QString& parent, const QString name)
{

}

bool FileEvent::isEmpty()
{
	return false;
}
void FileEvent::log()
{

}

void parseLocalFileEvent(const QString& rawData)
{

}

void parseRemoteFileEvent(const QString& rawData)
{

}

FileEventHandler::FileEventHandler(FileEvent fileEvent, QObject *parent)
	: QThread(parent)
	, fileEvent(fileEvent)
{

}




}