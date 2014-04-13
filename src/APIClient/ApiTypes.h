#ifndef API_TYPES_H
#define API_TYPES_H

#include <QtCore/QJsonDocument>
#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QVariant>
#include <QtGui/QPixmap>

class QJsonDocument;

namespace Drive
{

struct UserGroup
{
	static const QString paramId;
	static const QString paramName;

	static UserGroup fromJson(const QJsonObject& jsonObject);
	bool isValid() const;

	QString id;
	QString name;
};

struct Workspace
{
	static const QString paramId;
	static const QString paramName;
	static const QString paramPubId;
	static const QString paramUserChannel;
	static const QString paramServiceChannel;
	static const QString paramAvatar;
	static const QString paramGroups;

	static Workspace fromJson(const QJsonObject& jsonObject);
	const QString& serviceNotificationChannel() const { return serviceChannel; };
	bool isValid() const;

	int id;
	QString name;
	QString pubId;
	QString userChannel;
	QString serviceChannel;
	QString avatarUrl;
	QList<UserGroup> groups;
};

struct ProfileData
{
	static const QString paramId;
	static const QString paramEmail;
	static const QString paramFirstName;
	static const QString paramLastName;
	static const QString paramMiddleName;
	static const QString paramPhone;
	static const QString paramAvatar;
	static const QString paramSource;
	static const QString paramCreatedAt;
	static const QString paramModifiedAt;
	static const QString paramIsActive;
	static const QString paramIsDeleted;
	static const QString paramWorkspaces;

	static ProfileData fromJson(const QJsonObject& jsonObject);
	bool isValid();
	const Workspace defaultWorkspace() const;

	void log();

	QString id;
	QString email;
	QString firstName;
	QString lastName;
	QString middleName;
	QString phone;
	QString avatarUrl;
	QPixmap avatar;
	QString source;

	QDateTime createdAt;
	QDateTime modifiedAt;

	bool isActive;
	bool isDeleted;

	QList<Workspace> workspaces;
	//Workspace defaultWorkspace;
};

struct RemoteFileDesc
{
	enum FileType
	{
		File = 0,
		Dir = 1
	};

	static RemoteFileDesc fromJson(const QJsonObject& jsonObject);
	bool isValid() const;
	QString typeName() const;

	QJsonDocument toJson() const;
	QString toString() const;

	int id;
	int parentId;
	FileType type;
	QString name;
	quint64 size;
	uint createdAt;
	uint modifiedAt;
	uint deletedAt;
	QString checkSum;
	bool isFavourite;
	bool hasChildren;
	bool hasSubfolders;
	bool isUploaded;
	QString linkId;
	QString originalPath;
};

struct RemoteFileEvent
{
	enum EventType
	{
		Undefined = 0,
		Created,
		Uploaded,
		Modified,
		Trashed,
		Copied,
		Moved,
		Renamed,
		Restored,
		Deleted
	};

	static RemoteFileEvent fromJson(const QJsonObject& jsonObject);
	bool isValid() const;
	void log() const;
	void logCompact() const;
	QString typeName() const;

	static EventType eventNameToType(const QString& eventName);
	static QString typeName(EventType eventType);

	EventType type;
	QString originName;
	int targetId;
	int sourceId;
	int workspaceId;
	QString projectId;
	//uint timestamp;
	//qint64 timestampLow;
	QString timestamp;
	uint unixtime;
	RemoteFileDesc fileDesc;
};

class RemoteFileEventExclusion
{
public:
	enum IdMatchType
	{
		SelfId = 0,
		ParentId
	};

	RemoteFileEventExclusion() {};

	explicit RemoteFileEventExclusion(RemoteFileEvent::EventType eventType,
		int id, IdMatchType matchType = SelfId);

	inline bool operator==(const RemoteFileEventExclusion& rhs);

	int id() const;
	RemoteFileEvent::EventType eventType() const;
	IdMatchType matchType() const;

	bool matches(const RemoteFileEvent &event) const;
	void log() const;

private:
	IdMatchType m_matchType;
	RemoteFileEvent::EventType m_eventType;
	int m_id;
};

}

Q_DECLARE_METATYPE(Drive::RemoteFileEvent);
Q_DECLARE_METATYPE(Drive::RemoteFileDesc);
Q_DECLARE_METATYPE(QList<Drive::RemoteFileDesc>);
Q_DECLARE_METATYPE(Drive::RemoteFileEventExclusion);

#endif // API_TYPES_H
