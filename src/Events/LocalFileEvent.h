#ifndef LOCAL_FILE_EVENT
#define LOCAL_FILE_EVENT

#include <QtCore/QString>
#include <QtCore/QMetaType>

namespace Drive
{

class LocalFileEvent
{
public:
	enum Type
	{
		Added = 0,
		Modified,
		Deleted,
		Moved
	};

	LocalFileEvent();
	LocalFileEvent(Type type,
			const QString& dir,
			const QString& filePath = QString::null,
			const QString& oldFileName = QString::null);

	Type type() const;
	uint timeStamp() const;

	QString fileName() const;
	QString localPath() const;
	QString oldLocalPath() const;

	LocalFileEvent copyTo(Type type) const;

private:
	LocalFileEvent(Type type, const LocalFileEvent& source);

private:
	Type m_type;
	QString m_dir;
	QString m_filePath;
	QString m_oldFileName;
	uint m_timeStamp;
};

class LocalFileEventExclusion
{
public:
	enum PathMatchType
	{
		FullMatch = 0,
		PartialMatch
	};

	LocalFileEventExclusion() {}

	explicit LocalFileEventExclusion(LocalFileEvent::Type eventType,
		const QString& path, PathMatchType matchType = FullMatch);

	inline bool operator==(const LocalFileEventExclusion& rhs);

	PathMatchType matchType() const;
	LocalFileEvent::Type eventType() const;
	QString path() const;

	bool matches(const LocalFileEvent &event) const;

private:
	PathMatchType m_matchType;
	LocalFileEvent::Type m_eventType;
	QString m_path;
};

}

Q_DECLARE_METATYPE(Drive::LocalFileEvent)
Q_DECLARE_METATYPE(Drive::LocalFileEventExclusion)

#endif
