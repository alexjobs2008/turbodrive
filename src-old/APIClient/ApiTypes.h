#ifndef API_TYPES_H
#define API_TYPES_H

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
    bool isValid();
    
    QString id;
    QString name;
};

struct Workspace
{    
    static const QString paramId;
    static const QString paramName;
    static const QString paramPubId;
    static const QString paramGroups;

    static Workspace fromJson(const QJsonObject& jsonObject);
    bool isValid();
    
    QString id;
    QString name;
    QString pubId;
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
    static const QString paramUserChannel;
    static const QString paramServiceChannel;
    static const QString paramCreatedAt;
    static const QString paramModifiedAt;
    static const QString paramIsActive;
    static const QString paramIsDeleted;
    static const QString paramWorkspaces;
    
    static ProfileData fromJson(const QJsonObject& jsonObject);
    bool isValid();

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
    QString userChannel;
    QString serviceChannel;

    QDateTime createdAt;
    QDateTime modifiedAt;

    bool isActive;
    bool isDeleted;

    QList<Workspace> workspaces;
    Workspace defaultWorkspace;
};

struct RemoteFileDesc
{
	enum FileType
	{
		File = 0,
		Folder = 1
	};	

	static RemoteFileDesc fromJson(const QJsonObject& jsonObject);	
    bool isValid();
	void log();

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
		Modified,
		Trashed,
		Copied,
		Moved,
		Renamed,
		Restored,
		Deleted
	};

	RemoteFileEvent fromJson(const QJsonObject& jsonObject);
	bool isValid();
	void log();

	static EventType eventNameToType(const QString& eventName);
	static QString typeToEventName(EventType eventType);

	EventType type;
	int targetId;
	int workspaceId;
	int projectId;
	QString timestamp;
	RemoteFileDesc fileDesc;
};


}

#endif // API_TYPES_H