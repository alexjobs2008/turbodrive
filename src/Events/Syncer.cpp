#include "Syncer.h"
#include "APIClient/FilesService.h"
#include "Application/AppController.h"
#include "Settings/settings.h"
#include "QsLog/QsLog.h"
#include "Events/Cache.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>

namespace Drive
{

Syncer::Syncer()
	: QObject(nullptr)
	, m_currentLocalPathPrefix(QString())
	, m_folderCounter(0)
{
}

void Syncer::fullSync()
{
	m_localEvents.clear();
	m_remoteEvents.clear();

	getRoots();
}

void Syncer::onGetChildrenSucceeded(const QList<RemoteFileDesc>& list)
{
	--m_folderCounter;

	Q_FOREACH(RemoteFileDesc fileDesc, list)
	{
		if (fileDesc.type == RemoteFileDesc::Dir)
		{
			RemoteFileEvent event;
			event.type = RemoteFileEvent::Created;
			event.fileDesc = fileDesc;
			event.unixtime = QDateTime::currentDateTimeUtc().toTime_t();
			event.timestamp = QString::number(event.unixtime);
			event.projectId = "turbodrive"; //TODO: add to defines
			//event.workspaceId =
			//  AppController::instance().profileData().defaultWorkspace().id;

			QLOG_TRACE() << "SYNCER fileDesc: ";
			event.log();

			//emit newRemoteEvent(event);
			m_remoteEvents << event;

			emit newFile(fileDesc);
		}
	}

	Q_FOREACH(RemoteFileDesc fileDesc, list)
	{
		if (fileDesc.type == RemoteFileDesc::File)
		{
			RemoteFileEvent event;
			event.type = RemoteFileEvent::Uploaded;
			event.fileDesc = fileDesc;
			event.unixtime = QDateTime::currentDateTimeUtc().toTime_t();
			event.timestamp = QString::number(event.unixtime);
			event.projectId = "turbodrive"; //TODO: add to defines
			//event.workspaceId =
			//  AppController::instance().profileData().defaultWorkspace().id;

			QLOG_TRACE() << "SYNCER folder fileDesc: ";
			event.log();

			//emit newRemoteEvent(event);
			m_remoteEvents << event;

			emit newFile(fileDesc);
		}
	}

	Q_FOREACH(RemoteFileDesc fileDesc, list)
	{
		if (fileDesc.type == RemoteFileDesc::Dir)
			if (fileDesc.hasChildren)
			{
				GetChildrenResourceRef getChildrenRes =
					GetChildrenResource::create();

				connect(getChildrenRes.data(), &GetChildrenResource::succeeded,
					this, &Syncer::onGetChildrenSucceeded);

				connect(getChildrenRes.data(), &GetChildrenResource::failed,
					this, &Syncer::onGetFailed);

				++m_folderCounter;
				getChildrenRes->getChildren(fileDesc.id);
			}
	}

	if (!m_folderCounter)
	{
		fireEvents();
	}
}

void Syncer::getRoots()
{
	GetChildrenResourceRef getChildrenRes =
		GetChildrenResource::create();

	connect(getChildrenRes.data(), &GetChildrenResource::succeeded,
		this, &Syncer::onGetRootsSucceeded);

	connect(getChildrenRes.data(), &GetChildrenResource::failed,
		this, &Syncer::onGetFailed);

	getChildrenRes->getChildren(0);
}

void Syncer::getChildren()
{
	GetChildrenResourceRef getChildrenRes =
			GetChildrenResource::create();

	connect(getChildrenRes.data(), &GetChildrenResource::succeeded,
		this, &Syncer::onGetChildrenSucceeded);

	connect(getChildrenRes.data(), &GetChildrenResource::failed,
		this, &Syncer::onGetFailed);

	++m_folderCounter;

	const int diskId = -2;
	getChildrenRes->getChildren(diskId);

	syncLocalFolder(
		Settings::instance().get(Settings::folderPath).toString());
}

void Syncer::onGetRootsSucceeded(const QList<RemoteFileDesc>& roots)
{
	Q_FOREACH(const RemoteFileDesc& r, roots)
	{
		Q_EMIT newRoot(r);
	}

	getChildren();
}

void Syncer::onGetFailed() const
{
}

void Syncer::syncLocalFolder(const QString& localFolderPath)
{
	QDir dir(localFolderPath);

	if (dir.exists())
	{
		Q_FOREACH(QFileInfo info,
			dir.entryInfoList(QDir::NoDotAndDotDot
			| QDir::System
			//| QDir::Hidden
			| QDir::AllDirs
			| QDir::Files, QDir::DirsFirst))
		{
//			if (notifyLocalWatcher)
//			{
//				LocalFileEventNotifier::instance()
//					.addExclusion(info.absoluteFilePath());
//			}

			if (info.isDir())
			{
				if (QDir::cleanPath(info.absoluteFilePath())
					!= QDir::cleanPath(Settings::instance().get(Settings::folderPath).toString()))
				{
					m_localEvents << LocalFileEvent(LocalFileEvent::Added,
							QDir::cleanPath(info.absolutePath()),
							info.fileName());
					syncLocalFolder(info.absoluteFilePath());
				}
			}
			else
			{
				m_localEvents << LocalFileEvent(LocalFileEvent::Added,
						QDir::cleanPath(info.absolutePath()),
						info.fileName());
			}

		}
	}
}

void Syncer::fireEvents()
{
	for (int i = 0; i < m_localEvents.size(); ++i)
	{
		emit newLocalEvent(m_localEvents.at(i));
	}

	for (int i = 0; i < m_remoteEvents.size(); ++i)
	{
		emit newRemoteEvent(m_remoteEvents.at(i));
	}
}

}
