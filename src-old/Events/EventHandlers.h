#ifndef EVENT_TYPES_H
#define EVENT_TYPES_H

#include <QtCore/QThread>
#include <QtCore/QString>

namespace Drive
{

class FileEvent
{
public:	

	enum Source
	{
		Local = 0,
		Remote
	};

	enum EventType
	{
		Created = 0,
		Updated,
		Removed		
	};

	FileEvent(const QString& json);
	FileEvent(EventType type, const QString& parent, const QString name);

	bool isEmpty();
	void log();

private:
	void parseLocalFileEvent(const QString& rawData);
	void parseRemoteFileEvent(const QString& rawData);

	Source source;
//	FileType fileType;
	EventType eventType;
	
	QString name;
};

class FileEventHandler : public QThread
{
public:
	FileEventHandler(FileEvent fileEvent, QObject *parent = 0);

	bool process();

private:
	FileEvent fileEvent;
};

}

#endif // EVENT_TYPES_H