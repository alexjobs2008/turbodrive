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

Syncer::Syncer(QObject *parent)
	: QObject(parent)
	, currentLocalPathPrefix(QString())
	, folderCounter(0)
{
}

void Syncer::fullSync()
{
	localEvents.clear();
	remoteEvents.clear();

	getRoots();
}

void Syncer::onGetChildrenSucceeded(const QList<RemoteFileDesc>& list)
{
	--folderCounter;

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
			remoteEvents << event;

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
			remoteEvents << event;

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

				connect(getChildrenRes.data(),
					&GetChildrenResource::succeeded,
					this,
					&Syncer::onGetChildrenSucceeded);

				connect(getChildrenRes.data(), &GetChildrenResource::failed,
					this, &Syncer::onGetFailed);

				++folderCounter;
				getChildrenRes->getChildren(fileDesc.id);
			}
	}

	if (!folderCounter)
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

	++folderCounter;

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
	Q_ASSERT(false);
	QLOG_ERROR() << "Sync failed";
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

			QLOG_TRACE() << "info.filePath():" << info.filePath();

			if (info.isDir())
			{
				QLOG_TRACE() << "SYNCER is processing dir:"
					<< info.absoluteFilePath();
//				QLOG_TRACE() << QDir::toNativeSeparators(info.absoluteDir().absolutePath());
//				QLOG_TRACE() << QDir::toNativeSeparators(Settings::instance().get(Settings::folderPath).toString());


				if (QDir::toNativeSeparators(info.absoluteFilePath())
					!= QDir::toNativeSeparators(Settings::instance().get(Settings::folderPath).toString()))
				{
					LocalFileEvent event;
					event.type = LocalFileEvent::Added;
					event.dir =
						QDir::fromNativeSeparators(info.absoluteFilePath());
					event.filePath = QString();

					QLOG_TRACE() << "adding artificial event: ";
					event.logCompact();

//!					newLocalEvent(event);
					localEvents << event;

					syncLocalFolder(info.absoluteFilePath());
				}
			}
			else
			{
				LocalFileEvent event;
				event.type = LocalFileEvent::Added;
				event.dir = QDir::fromNativeSeparators(
					info.absoluteDir().absolutePath()).append(QDir::separator());
				event.filePath = info.fileName();

//!				newLocalEvent(event);
				localEvents << event;
			}

		}
	}
}

void Syncer::fireEvents()
{
	for (int i = 0; i < localEvents.size(); ++i)
	{
		emit newLocalEvent(localEvents.at(i));
	}

	for (int i = 0; i < remoteEvents.size(); ++i)
	{
		emit newRemoteEvent(remoteEvents.at(i));
	}
}

}
