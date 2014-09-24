#include "FileUtils.h"
#import <Cocoa/Cocoa.h>

bool setFolderIconFromPath(const char *folderURL, const char *iconPath)
{
    NSWorkspace *ws = [NSWorkspace sharedWorkspace];

    // Image
    NSString *iconURLString = [NSString stringWithCString:iconPath encoding:NSUTF8StringEncoding];
    NSURL* iconURL = [NSURL URLWithString:[iconURLString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
    NSImage *iconImage = [ws iconForFile: [iconURL path]];

    // Directory
    NSString *directoryURLString = [NSString stringWithCString:folderURL encoding:NSUTF8StringEncoding];
    NSURL* directoryURL = [NSURL URLWithString:[directoryURLString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];

    // Set icon
    BOOL didSetIcon = NO; // [ws setIcon:0 forFile:[directoryURL path] options:0];

    // Set icon
    didSetIcon = [ws setIcon:iconImage forFile:[directoryURL path] options:0];

    return didSetIcon;
}

bool setFolderIconFromQIcon(const char *folderURL, char *imageBytes, int imageSize)
{
    NSWorkspace *ws = [NSWorkspace sharedWorkspace];

    // Directory path
    NSString *directoryURLString = [NSString stringWithCString:folderURL encoding:NSUTF8StringEncoding];
    NSURL* directoryURL = [NSURL URLWithString:[directoryURLString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];

    // Image
    NSData *imageData = [NSData dataWithBytes:imageBytes length:imageSize];
    NSImage *iconImage = [[NSImage alloc] initWithData:imageData];

    // Set icon
    BOOL didSetIcon = [ws setIcon:0 forFile:[directoryURL path] options:0];

    /* NSError error;
    [[NSFileManager defaultManager] removeItemAtPath:filePath error:&error];
    NSLog(@"%@", error); */

    // Set icon
    didSetIcon = [ws setIcon:iconImage forFile:[directoryURL path] options:0];

    return didSetIcon;
}

extern CFStringRef kLSSharedFileListFavoriteVolumes;
extern CFStringRef kLSSharedFileListFavoriteItems;

bool addToFinderFavorites(const char *folder)
{
    // Directory path
    NSString *directoryURLString = [NSString stringWithCString:folder encoding:NSUTF8StringEncoding];
    NSURL* directoryURL = [NSURL URLWithString:[directoryURLString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
    CFURLRef directoryURLRef =  (CFURLRef)directoryURL;

    // NSURL* volumeUrl = [NSURL fileURLWithPath:@"/Volumes/MyVolume"];
    // [EBLaunchServices addItemWithURL:directoryURL toList:kLSSharedFileListFavoriteVolumes];

    LSSharedFileListRef list = LSSharedFileListCreate(NULL,
        /* (CFStringRef) */ kLSSharedFileListFavoriteVolumes, NULL);
    if (!list) return NO;

    NSLog(@"login_items are %@, url is %@, props are %s", list, directoryURL, "");

    LSSharedFileListItemRef item = LSSharedFileListInsertItemURL(list,
                                                                 kLSSharedFileListItemLast,
                                                                 NULL, NULL,
                                                                 directoryURLRef,
                                                                 NULL, NULL);
    NSLog(@"item is %@", item);

    CFRelease(list);
    return item ? (CFRelease(item), YES) : NO;

}

