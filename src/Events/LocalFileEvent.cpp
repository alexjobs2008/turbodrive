#include "LocalFileEvent.h"

#include "QsLog/QsLog.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>

namespace Drive
{

LocalFileEvent::LocalFileEvent()
{
    timestamp = QDateTime::currentDateTimeUtc().toTime_t();
}

QString LocalFileEvent::fileName() const
{
    QLOG_TRACE() << "LocalFileEvent::fileName():";
    this->logCompact();
    QLOG_TRACE() << "QDir::fromNativeSeparators(filePath):" << QDir::fromNativeSeparators(localPath());
    
    return QDir::fromNativeSeparators(localPath())
        .split("/", QString::SkipEmptyParts).last();
}

QString LocalFileEvent::localPath() const
{
    QString nativeDirPath = QDir::toNativeSeparators(dir);
    QString nativeFilePath = QDir::toNativeSeparators(filePath);

    if (!nativeDirPath.endsWith(QDir::separator())
        && !nativeFilePath.startsWith(QDir::separator()))
    {
        return nativeDirPath.append(QDir::separator()).append(nativeFilePath);
    }
    else
    {
        return nativeDirPath.append(nativeFilePath);
    }
}

QString LocalFileEvent::oldLocalPath() const
{
    return oldFileName.isEmpty()
        ? QString()
        : QDir::toNativeSeparators(dir).append(oldFileName);
}

void LocalFileEvent::log() const
{
    QString oldLocal = (!oldLocalPath().trimmed().isEmpty())
        ? QString("(%1)").arg(oldLocalPath()) : QString();
    
    QLOG_TRACE()
        << QDateTime::fromTime_t(timestamp).toString()
        << typeName(type)
        //<< localPath()
        << dir
        << filePath
        << oldLocal;
}

void LocalFileEvent::logCompact() const
{    
    QString sFileType;
    QString sLastModified;
    
    QFileInfo fileInfo(localPath());
    if (fileInfo.exists())
    {
        if (fileInfo.isDir())
        {
            sFileType = "Folder";
        }
        else if (fileInfo.isFile())
        {
            sFileType = "File";
        }
        else if (fileInfo.isSymLink())
        {
            sFileType = "SymLink";
        }
        else if (fileInfo.isBundle())
        {
            sFileType = "Bundle";
        }
        else
        {
            sFileType = "Unknown file type";
        }

        sLastModified = QString("(last modified: %1)")
            .arg(fileInfo.lastModified().toString());
    }    

    QLOG_TRACE () << "Local file event:"
        << sFileType
        << localPath()
        << "has been "
        << typeName(type)
        << sLastModified;
}

QString LocalFileEvent::typeName(EventType type)
{
    switch (type)
    {
    case Added:
        return "added";
    case Modified:
        return "modified";
    case Deleted:
        return "deleted";
    case Moved:
        return "moved";
    default:
        return QString();
    }
}

// ============================================================================

LocalFileEventExclusion::LocalFileEventExclusion(
    LocalFileEvent::EventType eventType
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

LocalFileEvent::EventType LocalFileEventExclusion::eventType() const
{
    return m_eventType;
}

QString LocalFileEventExclusion::path() const
{
    return m_path;
}

bool LocalFileEventExclusion::matches(const LocalFileEvent &event) const
{
    if (m_eventType != event.type)
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

void LocalFileEventExclusion::log() const
{
    QLOG_TRACE() << "Local file event exclusion:"
        << LocalFileEvent::typeName(m_eventType)
        << m_path
        << ", match type:" << pathMatchTypeName();
}

QString LocalFileEventExclusion::pathMatchTypeName() const
{
    switch (m_matchType)
    {
    case Drive::LocalFileEventExclusion::FullMatch:
        return "Full match";
        break;
    case Drive::LocalFileEventExclusion::PartialMatch:
        return "Partial match";
        break;
    default:
        Q_ASSERT(false);
        return false;
    }
}

}