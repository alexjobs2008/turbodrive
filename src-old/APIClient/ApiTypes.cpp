#include "ApiTypes.h"

#include "Application/AppController.h"
#include "QsLog/QsLog.h"

#include <QtCore/QMap>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

namespace
{

QVariant objectValue(const QJsonObject& dataObject,
							const QString& paramName, const QVariant defaultValue = QVariant())
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

namespace Drive
{

const QString UserGroup::paramId("_id");
const QString UserGroup::paramName("name");

const QString Workspace::paramId("_id");
const QString Workspace::paramName("name");
const QString Workspace::paramPubId("_pub_id");
const QString Workspace::paramGroups("groups");

const QString ProfileData::paramId("_id");
const QString ProfileData::paramEmail("email");
const QString ProfileData::paramFirstName("first_name");
const QString ProfileData::paramLastName("last_name");
const QString ProfileData::paramMiddleName("middle_name");
const QString ProfileData::paramPhone("phone");
const QString ProfileData::paramAvatar("avatar");
const QString ProfileData::paramSource("source");
const QString ProfileData::paramUserChannel("user_channel");
const QString ProfileData::paramServiceChannel("service_channel");
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

bool UserGroup::isValid()
{
    return !id.isEmpty() && !name.isEmpty();
}

Workspace Workspace::fromJson(const QJsonObject& jsonObject)
{
    Workspace workspace;

    workspace.id = objectValue(jsonObject, paramId, QString()).toString();
    workspace.name = objectValue(jsonObject, paramName, QString()).toString();
    workspace.pubId = objectValue(jsonObject, paramPubId, QString()).toString();

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

bool Workspace::isValid()
{
    return !id.isEmpty() && !name.isEmpty() && groups.size();
}

ProfileData ProfileData::fromJson(const QJsonObject& jsonObject)
{
    ProfileData profileData;

    profileData.id =
        objectValue(jsonObject, paramId, QString()).toString();
    profileData.email =
        objectValue(jsonObject, paramEmail, QString()).toString();
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
    profileData.userChannel =
        objectValue(jsonObject, paramUserChannel, QString()).toString();
    profileData.serviceChannel =
        objectValue(jsonObject, paramServiceChannel, QString()).toString();
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
    return !id.isEmpty() && !email.isEmpty() && !firstName.isEmpty()
        && !lastName.isEmpty() && !serviceChannel.isEmpty()
        && workspaces.size();
}

void ProfileData::log()
{
    QLOG_TRACE () << "Profile data:" << "\n"
        << paramId << ": " << id << "\n"
        << paramEmail << ": " << email << "\n"
        << paramFirstName << ": " << firstName << "\n"
        << paramLastName << ": " << lastName << "\n"
        << paramMiddleName << ": " << middleName << "\n"
        << paramPhone << ": " << phone << "\n"
        << paramAvatar << ": " << avatarUrl << "\n"
        << paramUserChannel << ": " << userChannel << "\n"
        << paramServiceChannel << ": " << serviceChannel << "\n"
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
            << "workspace" << Workspace::paramPubId << ": " << w.pubId << "\n";

        for (int j = 0; j < w.groups.size(); j++)
        {
            UserGroup u = w.groups.at(j);
            
            QLOG_TRACE ()
                << "group" << UserGroup::paramId << ": " << u.id << "\n"
                << "workspace" << UserGroup::paramName << ": " << u.name << "\n";
        }
    }
}

// 	"object\": 
// 	   {\"id\":428,
// 	   \"type\":1,
// 	   \"name\":\"11111111111111111111\",
// 	   \"size\":0,
// 	   \"created\":1386159600,
// 	   \"modified\":1386159600,
// 	   \"deleted\":0,
// 	   \"checksum\":null,
// 	   \"is_favorite\":0,
// 	   \"has_children\":false,
// 	   \"has_subfolders\":false,
// 	   \"is_uploaded\":false,
// 	   \"link_id\":null,
// 	   \"parent_id\":421
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
	desc.hasSubfolders = objectValue(jsonObject, "has_subfolders", false).toBool();
	desc.isUploaded = objectValue(jsonObject, "is_uploaded", false).toBool();
	desc.linkId = objectValue(jsonObject, "link_id", QString()).toString();
	desc.originalPath = objectValue(jsonObject, "original_path", QString()).toString();	

	return desc;
}

bool RemoteFileDesc::isValid()
{
	return !name.isEmpty();
}

void RemoteFileDesc::log()
{
	QLOG_TRACE () << "Remote file object descriptor:" << "\n"
		<< "id: " << id << "\n"
		<< "parentId: " << parentId << "\n"
		<< "type: " << (int) type << "\n"
		<< "name: " << name << "\n"
		<< "size: " << size << "\n"
		<< "createdAt: " << createdAt << "\n"
		<< "modifiedAt: " << modifiedAt << "\n"
		<< "deletedAt: " << deletedAt << "\n"
		<< "checksum: " << checkSum << "\n"
		<< "isFavourite: " << isFavourite << "\n"
		<< "hasChildren: " << hasChildren << "\n"
		<< "hasSubfolders: " << ": " << hasSubfolders << "\n"
		<< "isUploaded: " << isUploaded << "\n"
		<< "lindId: " << linkId << "\n";
}


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
				AppController::instance().profileData().serviceChannel;

			remoteEvent.timestamp = objectValue(idsObject,
				currentServiceChannel, QString()).toString();
		}
	}

	if (jsonObject.contains("data"))
	{
		QJsonValue data = jsonObject.value("data");
		if (data.type() == QJsonValue::Object)
		{
			QJsonObject dataObject = data.toObject();

			remoteEvent.workspaceId = 
				objectValue(dataObject, "workspaceId", 0).toInt();

			remoteEvent.projectId = 
				objectValue(dataObject, "projectId", 0).toInt();

			remoteEvent.type = eventNameToType(
				objectValue(dataObject, "eventName", QString()).toString());

			remoteEvent.targetId = 
				objectValue(dataObject, "targetId", 0).toInt();
				
			if (jsonObject.contains("object"))
			{
				QJsonValue obj = jsonObject.value("object");
				if (obj.type() == QJsonValue::Object)
				{
					remoteEvent.fileDesc = RemoteFileDesc::fromJson(obj.toObject());
				}
			}
		}
	}

	return remoteEvent;
}

bool RemoteFileEvent::isValid()
{
	return true;
}

RemoteFileEvent::EventType RemoteFileEvent::eventNameToType(const QString& eventName)
{
	if (eventName == "create")
	{
		return EventType::Created;
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

QString RemoteFileEvent::typeToEventName(EventType eventType)
{
	if (eventType == EventType::Created)
	{
		return "created";
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

void RemoteFileEvent::log()
{
	QLOG_TRACE () << "Remote file event:" << "\n"
		<< "timestamp: " << timestamp << "\n"
		<< "event type: " << typeToEventName(type) << "\n"
		<< "workspaceId: " << workspaceId << "\n"
		<< "projectId: " << projectId << "\n"
		<< "targetId: " << targetId << "\n";
	
	fileDesc.log();		
}


}