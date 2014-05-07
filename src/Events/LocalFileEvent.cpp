#include "LocalFileEvent.h"

#include "Util/FileUtils.h"

#include "QsLog/QsLog.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>

namespace Drive
{

LocalFileEvent::LocalFileEvent()
	: m_type(Added)
	, m_timeStamp(QDateTime::currentDateTimeUtc().toTime_t())
{
}

LocalFileEvent::LocalFileEvent(const Type type,
		const QString& dir, const QString& filePath, const QString& oldFileName)
	: m_type(type)
	, m_dir(dir)
	, m_filePath(filePath)
	, m_oldFileName(oldFileName)
	, m_timeStamp(QDateTime::currentDateTimeUtc().toTime_t())
{
}

LocalFileEvent::Type LocalFileEvent::type() const
{
	return m_type;
}

uint LocalFileEvent::timeStamp() const
{
	return m_timeStamp;
}

QString LocalFileEvent::fileName() const
{
	return m_filePath.split(Utils::separator(), QString::SkipEmptyParts).last();
}

QString LocalFileEvent::localPath() const
{
	return m_dir + Utils::separator() + m_filePath;
}

QString LocalFileEvent::oldLocalPath() const
{
	return m_oldFileName.isEmpty()
		? QString::null
		: m_dir + Utils::separator() + m_oldFileName;
}

LocalFileEvent LocalFileEvent::copyTo(LocalFileEvent::Type type) const
{
	return LocalFileEvent(type, *this);
}

LocalFileEvent::LocalFileEvent(LocalFileEvent::Type type, const LocalFileEvent& source)
	: m_type(type)
	, m_dir(source.m_dir)
	, m_filePath(source.m_filePath)
	, m_oldFileName(source.m_oldFileName)
	, m_timeStamp(source.m_timeStamp)
{
}

// ============================================================================

LocalFileEventExclusion::LocalFileEventExclusion(
	LocalFileEvent::Type eventType
	, const QString& path
	, PathMatchType matchType)
	: m_eventType(eventType)
	, m_path(path)
	, m_matchType(matchType)
{
}

bool LocalFileEventExclusion::operator==(const LocalFileEventExclusion& rhs)
{
	return m_eventType == rhs.eventType()
		&& m_path == rhs.path()
		&& m_matchType == rhs.matchType();
}

LocalFileEventExclusion::PathMatchType
	LocalFileEventExclusion::matchType() const
{
	return m_matchType;
}

LocalFileEvent::Type LocalFileEventExclusion::eventType() const
{
	return m_eventType;
}

QString LocalFileEventExclusion::path() const
{
	return m_path;
}

bool LocalFileEventExclusion::matches(const LocalFileEvent &event) const
{
	if (m_eventType != event.type())
		return false;

	switch (m_matchType)
	{
	case Drive::LocalFileEventExclusion::FullMatch:
		return m_path == event.localPath();
	case Drive::LocalFileEventExclusion::PartialMatch:
		return event.localPath().startsWith(m_path, Qt::CaseSensitive);
	default:
		Q_ASSERT(false);
		return false;
		break;
	}
}

}
