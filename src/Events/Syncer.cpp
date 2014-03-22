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

	GetChildrenResourceRef getChildrenRes =
		GetChildrenResource::create();

	connect(getChildrenRes.data(), SIGNAL(succeeded(QList<Drive::RemoteFileDesc>)),
		this, SLOT(onGetChildrenSucceeded(QList<Drive::RemoteFileDesc>)));

	connect(getChildrenRes.data(), SIGNAL(failed()),
		this, SLOT(onGetChildrenFailed()));

	++folderCounter;
	getChildrenRes->getChildren(-2);

//	LocalCache::instance().log();

	syncLocalFolder(
		Settings::instance().get(Settings::folderPath).toString());
}

void Syncer::onGetChildrenSucceeded(QList<RemoteFileDesc> list)
{
	--folderCounter;

	foreach (RemoteFileDesc fileDesc, list)
	{
		if (fileDesc.type == RemoteFileDesc::Folder)
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

			emit newFileDesc(fileDesc);
		}
	}

	foreach (RemoteFileDesc fileDesc, list)
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

			emit newFileDesc(fileDesc);
		}
	}

	foreach (RemoteFileDesc fileDesc, list)
	{
		if (fileDesc.type == RemoteFileDesc::Folder)
			if (fileDesc.hasChildren)
			{
				GetChildrenResourceRef getChildrenRes =
					GetChildrenResource::create();

				connect(getChildrenRes.data(),
					SIGNAL(succeeded(QList<Drive::RemoteFileDesc>)),
					this,
					SLOT(onGetChildrenSucceeded(QList<Drive::RemoteFileDesc>)));

				connect(getChildrenRes.data(), SIGNAL(failed()),
					this, SLOT(onGetChildrenFailed()));

				++folderCounter;
				getChildrenRes->getChildren(fileDesc.id);
			}
	}

	if (!folderCounter)
	{
		fireEvents();
	}
}

void Syncer::onGetChildrenFailed()
{
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