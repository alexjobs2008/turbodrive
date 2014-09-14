
#include <Cocoa/Cocoa.h>

#ifdef __cplusplus
extern "C" {
#endif

void setDockIconVisibility(bool visible)
{visible=true;
    /* ProcessSerialNumber psn;
    if (GetCurrentProcess(&psn) == noErr)
    {
        TransformProcessType(&psn,
                             visible ? kProcessTransformToForegroundApplication :
                                       kProcessTransformToBackgroundApplication);
        // kProcessTransformToUIElementApplication
    } */
}

int addPathToSharedItem(char *path, char *displayName)
{
    CFStringRef pathStr = CFStringCreateWithCString(kCFAllocatorDefault, path, kCFStringEncodingUTF8);
    CFURLRef pathURL = CFURLCreateWithString(kCFAllocatorDefault, pathStr, NULL);
    CFStringRef displayNameStr = CFStringCreateWithCString(kCFAllocatorDefault, displayName, kCFStringEncodingUTF8);

    // Create a reference to the shared file list.
    LSSharedFileListRef favoriteItems = LSSharedFileListCreate(
            NULL, kLSSharedFileListFavoriteItems, NULL);

    if (favoriteItems)
    {
        // Insert an item to the list.
        LSSharedFileListItemRef item = LSSharedFileListInsertItemURL(
                    favoriteItems, kLSSharedFileListItemLast, displayNameStr,
                    NULL, pathURL, NULL, NULL);
        if (item)
        {
            CFRelease(item);
        }
        else
        {
            return -2;
        }

        CFRelease(favoriteItems);
    }
    else
    {
        return -1;
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
