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
    BOOL didSetIcon = [ws setIcon:iconImage forFile:[directoryURL path] options:0];

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
    BOOL didSetIcon = [ws setIcon:iconImage forFile:[directoryURL path] options:0];

    return didSetIcon;
}
