#pragma once

// No state set for file or folder
#define FOLDER_STATE_NOT_SET 0

// Corresponds to folder or file state
#define FOLDER_ICON_OK 1
#define FOLDER_ICON_ERROR 2
#define FOLDER_ICON_SYNC 3

#define FOLDER_STATE_DELETED 100

int getState(wchar_t *fileName);
