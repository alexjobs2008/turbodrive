#include "EventHandlerBase.h"
#include "FileUtils.h"


void Drive::EventHandlerBase::markSyncing(const QString &fileName)
{
    this->fileName = fileName;

    if (state == Drive::FOLDER_STATE_NOT_SET)
    {
        FolderIconController::instance().setState(this->fileName, Drive::FOLDER_ICON_SYNC);
        state = Drive::FOLDER_ICON_SYNC;
    }
}

void Drive::EventHandlerBase::markOk()
{
    if (state == Drive::FOLDER_ICON_SYNC)
    {
        FolderIconController::instance().setState(fileName, Drive::FOLDER_ICON_OK);
        state = Drive::FOLDER_ICON_OK;
    }
}

void Drive::EventHandlerBase::markError()
{
    if (state == Drive::FOLDER_ICON_SYNC)
    {
        FolderIconController::instance().setState(fileName, Drive::FOLDER_ICON_ERROR);
        state = Drive::FOLDER_ICON_ERROR;
    }
}

void Drive::EventHandlerBase::markDeleted()
{
    FolderIconController::instance().setDeleted(fileName);
    state = Drive::FOLDER_STATE_NOT_SET;
}
