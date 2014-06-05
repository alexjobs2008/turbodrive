#include "ApiTypes.h"

#include "Application/AppController.h"
#include "QsLog/QsLog.h"

#include <QtCore/QMap>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonParseError>

#define RPL_CHANNEL_PREFIX "th_"
#define RPL_CHANNEL_POSTFIX "_turbodrive"

namespace
{

QVariant objectValue(const QJsonObject& dataObject,
					const QString& paramName,
					const QVariant defaultValue = QVariant())
{
	if (dataObject.contains(paramName))
	{
		QJsonValue value = dataObject.value(paramName);
		switch (value.type())
		{
		case QJsonValue::Null:
		case QJsonValue::Undefined:
			return QVariant();
		case QJsonValue::Double:
			return /*(int)*/ value.toDouble();
		case QJsonValue::String:
			return value.toString();
		case QJsonValue::Bool:
			return value.toBool();
			break;
		case QJsonValue::Object:
		case QJsonValue::Array:
		default:
			return value.toVariant();
		}
	}
	return defaultValue;
}

} // anonymous namespace

// ============================================================================

namespace Drive
{

const QString UserGroup::paramId("_id");
const QString UserGroup::paramName("name");

const QString Workspace::paramId("_id");
const QString Workspace::paramName("name");
const QString Workspace::paramPubId("_pub_id");
const QString Workspace::paramUserChannel("user_channel");
const QString Workspace::paramServiceChannel("service_channel");
const QString Workspace::paramGroups("groups");

const QString ProfileData::paramId("id");
const QString ProfileData::paramUsername("username");
const QString ProfileData::paramFirstName("firstname");
const QString ProfileData::paramLastName("lastname");
const QString ProfileData::paramMiddleName("middle_name");
const QString ProfileData::paramPhone("phone");
const QString ProfileData::paramAvatar("avatar");
const QString ProfileData::paramSource("source");
const QString ProfileData::paramCreatedAt("created_at");
const QString ProfileData::paramModifiedAt("updated_at");
const QString ProfileData::paramIsActive("is_active");
const QString ProfileData::paramIsDeleted("deleted");
const QString ProfileData::paramWorkspaces("workspaces");

UserGroup UserGroup::fromJson(const QJsonObject& jsonObject)
{
	UserGroup userGroup;

	userGroup.id = objectValue(jsonObject, paramId, QString()).toString();
	userGroup.name = objectValue(jsonObject, paramName, QString()).toString();

	return userGroup;
}

bool UserGroup::isValid() const
{
	return !id.isEmpty() && !name.isEmpty();
}

Workspace Workspace::fromJson(const QJsonObject& jsonObject)
{
	Workspace workspace;

	workspace.id = objectValue(jsonObject, paramId, 0).toInt();
	workspace.name = objectValue(jsonObject, paramName, QString()).toString();
	workspace.pubId =
		objectValue(jsonObject, paramPubId, QString()).toString();

	workspace.userChannel = QString("%1%2")
		.arg(RPL_CHANNEL_PREFIX)
		.arg(objectValue(jsonObject, paramUserChannel, QString()).toString())
		.arg(RPL_CHANNEL_POSTFIX);

	workspace.serviceChannel = QString("%1%2%3")
		.arg(RPL_CHANNEL_PREFIX)
		.arg(objectValue(jsonObject, paramServiceChannel,
			QString()).toString())
		.arg(RPL_CHANNEL_POSTFIX);


	if (jsonObject.contains(paramGroups))
	{
		if (jsonObject.value(paramGroups).type() == QJsonValue::Object)
		{
			QJsonObject groupsObject =
				jsonObject.value(paramGroups).toObject();

			QStringList list =
				jsonObject.value(paramGroups).toObject().keys();

			for (int i = 0; i < list.size(); i++)
			{
				QJsonValue arrayValue = groupsObject.value(list.at(i));

				if (arrayValue.type() == QJsonValue::Object)
				{
					UserGroup userGroup =
						UserGroup::fromJson(arrayValue.toObject());

					if (userGroup.isValid())
					{
						workspace.groups << userGroup;
					}
				}
			}
		}
	}

	return workspace;
}

bool Workspace::isValid() const
{
	return id;
}

// ============================================================================

ProfileData ProfileData::fromJson(const QJsonObject& jsonObject)
{
	ProfileData profileData;

	profileData.id =
		objectValue(jsonObject, paramId, QString()).toString();
	profileData.username =
		objectValue(jsonObject, paramUsername, QString()).toString();
	profileData.firstName =
		objectValue(jsonObject, paramFirstName, QString()).toString();
	profileData.lastName =
		objectValue(jsonObject, paramLastName, QString()).toString();
	profileData.middleName =
		objectValue(jsonObject, paramMiddleName, QString()).toString();
	profileData.phone =
		objectValue(jsonObject, paramPhone, QString()).toString();
	profileData.avatarUrl =
		objectValue(jsonObject, paramAvatar, QString()).toString();
	profileData.avatar = QPixmap();
	profileData.source =
		objectValue(jsonObject, paramSource, QString()).toString();

	profileData.createdAt = QDateTime::fromTime_t(
		objectValue(jsonObject, paramCreatedAt, 0).toUInt());
	profileData.modifiedAt = QDateTime::fromTime_t(
		objectValue(jsonObject, paramModifiedAt, 0).toUInt());
	profileData.isActive =
		objectValue(jsonObject, paramIsActive, false).toBool();
	profileData.isDeleted =
		objectValue(jsonObject, paramIsDeleted, true).toBool();

	if (jsonObject.contains(paramWorkspaces))
	{
		if (jsonObject.value(paramWorkspaces).type() == QJsonValue::Object)
		{
			QJsonObject workspacesObject =
				jsonObject.value(paramWorkspaces).toObject();

			QStringList list =
				jsonObject.value(paramWorkspaces).toObject().keys();

			for (int i = 0; i < list.size(); i++)
			{
				QJsonValue arrayValue = workspacesObject.value(list.at(i));

				if (arrayValue.type() == QJsonValue::Object)
				{
					Workspace workspace =
						Workspace::fromJson(arrayValue.toObject());

					if (workspace.isValid())
					{
						profileData.workspaces << workspace;
					}
				}
			}
		}
	}

	return profileData;
}

bool ProfileData::isValid()
{
	return !id.isEmpty() && !username.isEmpty() /*&& !firstName.isEmpty()
		&& !lastName.isEmpty() */ && workspaces.size();
}

const Workspace ProfileData::defaultWorkspace() const
{
	if (workspaces.size())
	{
		return workspaces.at(0);
	}
	else
	{
		return Workspace();
	}
}

void ProfileData::log()
{
	QLOG_TRACE () << "Profile data:" << "\n"
		<< paramId << ": " << id << "\n"
		<< paramUsername << ": " << username << "\n"
		<< paramFirstName << ": " << firstName << "\n"
		<< paramLastName << ": " << lastName << "\n"
		<< paramMiddleName << ": " << middleName << "\n"
		<< paramPhone << ": " << phone << "\n"
		<< paramAvatar << ": " << avatarUrl << "\n"
		<< paramCreatedAt << ": " << createdAt << "\n"
		<< paramModifiedAt << ": " << modifiedAt << "\n"
		<< paramIsActive << ": " << isActive << "\n"
		<< paramIsDeleted << ": " << isDeleted << "\n"
		<< "workspaces #" << ": " << workspaces.size() << "\n";

	for (int i = 0; i < workspaces.size(); i++)
	{
		Workspace w = workspaces.at(i);

		QLOG_TRACE ()
			<< "workspace" << Workspace::paramId << ": " << w.id << "\n"
			<< "workspace" << Workspace::paramName << ": " << w.name << "\n"
			<< "workspace" << Workspace::paramPubId << ": " << w.pubId << "\n"
			<< "workspace"
			<<  Workspace::paramUserChannel << ": " << w.userChannel << "\n"
			<< "workspace"
			<<  Workspace::paramServiceChannel
			<< ": " << w.serviceChannel << "\n";

		for (int j = 0; j < w.groups.size(); j++)
		{
			UserGroup u = w.groups.at(j);

			QLOG_TRACE ()
				<< "group" << UserGroup::paramId << ": " << u.id << "\n"
				<< "workspace"
				<< UserGroup::paramName << ": " << u.name << "\n";
		}
	}
}

// ============================================================================

// Example:
// 	"object\":
// 	{\"id\":428,
// 	\"type\":1,
// 	\"name\":\"11111111111111111111\",
// 	\"size\":0,
// 	\"created\":1386159600,
// 	\"modified\":1386159600,
// 	\"deleted\":0,
// 	\"checksum\":null,
// 	\"is_favorite\":0,
// 	\"has_children\":false,
// 	\"has_subfolders\":false,
// 	\"is_uploaded\":false,
// 	\"link_id\":null,
// 	\"parent_id\":421
// 	}
RemoteFileDesc RemoteFileDesc::fromJson(const QJsonObject& jsonObject)
{
	RemoteFileDesc desc;

	desc.id = objectValue(jsonObject, "id", 0).toInt();
	desc.parentId = objectValue(jsonObject, "parent_id", 0).toInt();
	desc.type = (FileType) objectValue(jsonObject, "type", 0).toInt();
	desc.name = objectValue(jsonObject, "name", QString()).toString();
	desc.size = objectValue(jsonObject, "size", 0).toULongLong();
	desc.createdAt = objectValue(jsonObject, "created", 0).toUInt();
	desc.modifiedAt = objectValue(jsonObject, "modified", 0).toUInt();
	desc.deletedAt = objectValue(jsonObject, "deleted", 0).toUInt();
	desc.checkSum = objectValue(jsonObject, "checksum", QString()).toString();
	desc.isFavourite = objectValue(jsonObject, "is_favorite", false).toBool();
	desc.hasChildren = objectValue(jsonObject, "has_children", false).toBool();
	desc.hasSubfolders =
		objectValue(jsonObject, "has_subfolders", false).toBool();
	desc.isUploaded = objectValue(jsonObject, "is_uploaded", false).toBool();
	desc.linkId = objectValue(jsonObject, "link_id", QString()).toString();
	desc.originalPath =
		objectValue(jsonObject, "original_path", QString()).toString();

	return desc;
}

bool RemoteFileDesc::isValid() const
{
	return !name.isEmpty() && id && parentId;
}

QString RemoteFileDesc::typeName() const
{
	switch (type)
	{
	case Drive::RemoteFileDesc::File:
		return "file";
	case Drive::RemoteFileDesc::Dir:
		return "folder";
	default:
		return "WRONG TYPE";
	}
}

QJsonDocument RemoteFileDesc::toJson() const
{
	QVariantMap map;

#define MAKE_PAIR(value) QLatin1String(#value), QVariant(value)
	map.insert(MAKE_PAIR(id));
	map.insert(MAKE_PAIR(parentId));
	map.insert(MAKE_PAIR(type));
	map.insert(MAKE_PAIR(name));
	map.insert(MAKE_PAIR(size));
	map.insert(MAKE_PAIR(createdAt));
	map.insert(MAKE_PAIR(modifiedAt));
	map.insert(MAKE_PAIR(deletedAt));
	map.insert(MAKE_PAIR(checkSum));
	map.insert(MAKE_PAIR(isFavourite));
	map.insert(MAKE_PAIR(hasChildren));
	map.insert(MAKE_PAIR(hasSubfolders));
	map.insert(MAKE_PAIR(isUploaded));
	map.insert(MAKE_PAIR(linkId));
	map.insert(MAKE_PAIR(originalPath));
#undef MAKE_PAIR

	return QJsonDocument(QJsonObject::fromVariantMap(map));
}

QString RemoteFileDesc::toString() const
{
	return toJson().toJson(QJsonDocument::Compact);
}

// ============================================================================

RemoteFileEvent RemoteFileEvent::fromJson(const QJsonObject& jsonObject)
{
	RemoteFileEvent remoteEvent;

	if (jsonObject.contains("ids"))
	{
		QJsonValue ids = jsonObject.value("ids");
		if (ids.type() == QJsonValue::Object)
		{
			QJsonObject idsObject = ids.toObject();

			QString currentServiceChannel =
				AppController::instance().serviceChannel();

			QString ts = objectValue(idsObject,
				currentServiceChannel, QString()).toString();

			remoteEvent.timestamp = ts;

			remoteEvent.unixtime = ts.split(".").at(0).toUInt();

//			remoteEvent.timestamp = ts.split(".").at(0).toUInt();
//			remoteEvent.timestampLow = ts.split(".").at(1).toLongLong();
		}
	}

	if (jsonObject.contains("data"))
	{
		QJsonValue data = jsonObject.value("data");

		if (data.type() == QJsonValue::String)
		{
			QJsonDocument doc =
				QJsonDocument::fromJson(data.toString().toUtf8());

			if (doc.isObject())
			{
				QJsonObject dataObject = doc.object();

				remoteEvent.workspaceId =
					objectValue(dataObject, "workspaceId", 0).toInt();

				remoteEvent.projectId =
					objectValue(dataObject, "projectId", QString()).toString();

				remoteEvent.type = eventNameToType(
					objectValue(dataObject, "eventName", QString())
					.toString());

				remoteEvent.targetId =
					objectValue(dataObject, "targetId", 0).toInt();

				remoteEvent.sourceId =
					objectValue(dataObject, "sourceId", 0).toInt();

				remoteEvent.originName =
					objectValue(dataObject, "originName", QString())
					.toString();

				if (dataObject.contains("object"))
				{
					QJsonValue obj = dataObject.value("object");

					QLOG_TRACE() << "obj : " << obj.toString();
					QLOG_TRACE() << "obj type: " << obj.type();

					if (obj.type() == QJsonValue::Object)
					{
						remoteEvent.fileDesc =
							RemoteFileDesc::fromJson(obj.toObject());
					}
				}
			}
		}
	}

	return remoteEvent;
}

bool RemoteFileEvent::isValid() const
{
	return (type != EventType::Undefined)
		&& (projectId == "turbodrive") // //TODO: add turbodrive to defines
		&& fileDesc.isValid();
}

RemoteFileEvent::EventType RemoteFileEvent::eventNameToType(
	const QString& eventName)
{
	if (eventName == "create")
	{
		return EventType::Created;
	}
	else if (eventName == "upload")
	{
		return EventType::Uploaded;
	}
	else if (eventName == "update")
	{
		return EventType::Modified;
	}
	else if (eventName == "trash")
	{
		return EventType::Trashed;
	}
	else if (eventName == "copy")
	{
		return EventType::Copied;
	}
	else if (eventName == "move")
	{
		return EventType::Moved;
	}
	else if (eventName == "rename")
	{
		return EventType::Renamed;
	}
	else if (eventName == "restore")
	{
		return EventType::Restored;
	}
	else if (eventName == "delete")
	{
		return EventType::Deleted;
	}
	else
	{
		return EventType::Undefined;
	}
}

QString RemoteFileEvent::typeName(EventType eventType)
{
	if (eventType == EventType::Created)
	{
		return "created";
	}
	else if (eventType == EventType::Uploaded)
	{
		return "uploaded";
	}
	else if (eventType == EventType::Modified)
	{
		return "modified";
	}
	else if (eventType == EventType::Trashed)
	{
		return "moved to trash";
	}
	else if (eventType == EventType::Copied)
	{
		return "copied";
	}
	else if (eventType == EventType::Moved)
	{
		return "moved";
	}
	else if (eventType == EventType::Renamed)
	{
		return "renamed";
	}
	else if (eventType == EventType::Restored)
	{
		return "restored";
	}
	else if (eventType == EventType::Deleted)
	{
		return "deleted";
	}
	else
	{
		return "undefined";
	}
}

void RemoteFileEvent::log() const
{
	QLOG_TRACE () << "Remote file event: " << "\n"
		<< "Valid:" << isValid() << "\n"
		<< "timestamp:" << timestamp << "\n"
//		<< "timestamp low:" << timestampLow << "\n"
		<< "event type:" << typeName(type) << "\n"
		<< "workspaceId:" << workspaceId << "\n"
		<< "projectId:" << projectId << "\n"
		<< "targetId:" << targetId << "\n"
		<< "fileDesc:" << fileDesc.toString();
}

void RemoteFileEvent::logCompact() const
{
	QString sFileType =
		fileDesc.type == RemoteFileDesc::File ? "File" : "Folder";

	QString lastModified =
		QString("(last modified: %1)")
		.arg(QDateTime::fromTime_t(fileDesc.modifiedAt).toString());

	QLOG_TRACE () << "Remote file event:"
		<< sFileType
		<< fileDesc.name
		<< "has been"
		<< typeName(type)
		<< lastModified;
}

QString RemoteFileEvent::typeName() const
{
	return typeName(type);
}

// ============================================================================

RemoteFileEventExclusion::RemoteFileEventExclusion(
	RemoteFileEvent::EventType eventType, int id, IdMatchType matchType)
	: m_matchType(matchType)
	, m_eventType(eventType)
	, m_id(id)
{
}

bool RemoteFileEventExclusion::operator==(const RemoteFileEventExclusion& rhs)
{
	return m_id == rhs.id()
		&& m_eventType == rhs.eventType()
		&& m_matchType == rhs.matchType();
}

int RemoteFileEventExclusion::id() const
{
	return m_id;
}

RemoteFileEvent::EventType RemoteFileEventExclusion::eventType() const
{
	return m_eventType;
}

RemoteFileEventExclusion::IdMatchType
	RemoteFileEventExclusion::matchType() const
{
	return m_matchType;
}

bool RemoteFileEventExclusion::matches(const RemoteFileEvent &event) const
{
	if (m_eventType != event.type)
		return false;

	if (!event.fileDesc.isValid())
	{
		QLOG_ERROR()
				<< "Skipping event exclusion check, as file descriptor is not valid:"
				<< event.fileDesc.toString();
		return false;
	}

	switch (m_matchType)
	{
	case Drive::RemoteFileEventExclusion::SelfId:
		return m_id == event.fileDesc.id;
	case Drive::RemoteFileEventExclusion::ParentId:
		return m_id == event.fileDesc.parentId;
	default:
		Q_ASSERT(false);
	}

	return false;
}

void RemoteFileEventExclusion::log() const
{
	QLOG_TRACE() << "Remote file event exclusion:"
		<< RemoteFileEvent::typeName(m_eventType)
		<< "," << m_id;
}

}
