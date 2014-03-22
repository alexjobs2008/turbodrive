#ifndef LOCAL_FILE_EVENT
#define LOCAL_FILE_EVENT

#include <QtCore/QString>
#include <QtCore/QMetaType>

namespace Drive
{

struct LocalFileEvent
{
public:
	enum EventType
	{
		Added = 0,
		Modified,
		Deleted,
		Moved
	};

	LocalFileEvent();

	QString fileName() const;
	QString localPath() const;
	QString oldLocalPath() const;

	void log() const;
	void logCompact() const;
	static QString typeName(EventType type);

//private:

	uint timestamp;
	EventType type;
	QString dir;
	QString filePath;
	QString oldFileName;
	int remoteFileId;
};

class LocalFileEventExclusion
{
public:
	enum PathMatchType
	{
		FullMatch = 0,
		PartialMatch
	};

	LocalFileEventExclusion() {};

	explicit LocalFileEventExclusion(LocalFileEvent::EventType eventType,
		const QString& path, PathMatchType matchType = FullMatch);

	inline bool operator==(const LocalFileEventExclusion& rhs);

	PathMatchType matchType() const;
	LocalFileEvent::EventType eventType() const;
	QString path() const;

	bool matches(const LocalFileEvent &event) const;
	void log() const;
	QString pathMatchTypeName() const;

private:
	//LocalFileEventExclusion() {};

	PathMatchType m_matchType;			// exclusion path match type
	LocalFileEvent::EventType m_eventType;  // local file event type
	QString m_path;						// local file path
};



}

Q_DECLARE_METATYPE(Drive::LocalFileEvent);
Q_DECLARE_METATYPE(Drive::LocalFileEventExclusion);

#endif LOCAL_FILE_EVENT
