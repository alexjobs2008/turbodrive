#include "EventHandlerBase.h"
#include "FileUtils.h"
#include "Events/LocalFileEvent.h"

void Drive::EventHandlerBase::markSyncing(const QString &fileName)
{
    this->fileName = fileName;

    if (syncronizationState != Drive::FOLDER_ICON_OK)
    {
        syncronizationState = Drive::FOLDER_ICON_SYNC;
        FolderIconController::instance().setState(this->fileName, Drive::FOLDER_ICON_SYNC);
        /* Q_EMIT newLocalFileEventExclusion(LocalFileEventExclusion(
                    LocalFileEvent::Modified, this->fileName)); */
    }
}

void Drive::EventHandlerBase::markOk()
{
    if (syncronizationState == Drive::FOLDER_ICON_SYNC)
    {
        syncronizationState = Drive::FOLDER_ICON_OK;
        FolderIconController::instance().setState(this->fileName, Drive::FOLDER_ICON_OK);
        /* Q_EMIT newLocalFileEventExclusion(LocalFileEventExclusion(
                    LocalFileEvent::Modified, this->fileName)); */
    }
}

void Drive::EventHandlerBase::markError()
{
    if (syncronizationState == Drive::FOLDER_ICON_SYNC)
    {
        syncronizationState = Drive::FOLDER_ICON_ERROR;
        FolderIconController::instance().setState(this->fileName, Drive::FOLDER_ICON_ERROR);
        /* Q_EMIT newLocalFileEventExclusion(LocalFileEventExclusion(
                    LocalFileEvent::Modified, this->fileName)); */
    }
}

void Drive::EventHandlerBase::markDeleted()
{
    FolderIconController::instance().setDeleted(fileName);
    syncronizationState = Drive::FOLDER_STATE_NOT_SET;
}
