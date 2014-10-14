#include "FileUtils.h"
#include <MacTypes.h>
#import <Cocoa/Cocoa.h>
#import <QuickLook/QuickLook.h>


//
// Utility functions
//

NSImage *resizeNSImage(NSImage *computerImage)
{
    // NSImage *computerImage = [NSImage imageNamed:NSImageNameComputer];
    NSInteger size = 256;

    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc]
                             initWithBitmapDataPlanes:NULL
                             pixelsWide:size
                             pixelsHigh:size
                             bitsPerSample:8
                             samplesPerPixel:4
                             hasAlpha:YES
                             isPlanar:NO
                             colorSpaceName:NSCalibratedRGBColorSpace
                             bytesPerRow:0
                             bitsPerPixel:0];
    [rep setSize:NSMakeSize(size, size)];

    [NSGraphicsContext saveGraphicsState];
    [NSGraphicsContext setCurrentContext:[NSGraphicsContext graphicsContextWithBitmapImageRep:rep]];
    [computerImage drawInRect:NSMakeRect(0, 0, size, size) fromRect:NSZeroRect operation:NSCompositeCopy fraction:1.0];
    [NSGraphicsContext restoreGraphicsState];

    NSData *data = [rep representationUsingType:NSPNGFileType properties:nil];
    NSImage *img = [[NSImage alloc] initWithData:data];

    return img;
}




//
// Header functions
//


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
    // NSURL* directoryURL = [NSURL URLWithString:directoryURLString]; // stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
    // CFURLRef directoryURLRef =  (CFURLRef)directoryURL;

    NSURL *nsurl = [NSURL fileURLWithPath:directoryURLString];
    CFURLRef url = (CFURLRef) nsurl;

    // NSURL* volumeUrl = [NSURL fileURLWithPath:@"/Volumes/MyVolume"];
    // [EBLaunchServices addItemWithURL:directoryURL toList:kLSSharedFileListFavoriteVolumes];

    LSSharedFileListRef list = LSSharedFileListCreate(NULL,
        /* (CFStringRef) */ kLSSharedFileListFavoriteItems, NULL);
    if (!list) return NO;

    NSLog(@"login_items are %@, url is %@, props are %s", list, url, "");

    LSSharedFileListItemRef item = LSSharedFileListInsertItemURL(list,
                                                                 kLSSharedFileListItemLast,
                                                                 (CFStringRef) @"МТС Диск", NULL,
                                                                 url, // directoryURLRef,
                                                                 NULL, NULL);
    NSLog(@"item is %@", item);

    CFRelease(list);
    return item ? (CFRelease(item), YES) : NO;

}

bool setBadgeIcon(const char *pathURL, /* NSData* tag */ char *imageBytes, int imageSize)
{
    FSCatalogInfo info;
    FSRef par;
    FSRef ref;
    Boolean dir = false;

    // Directory path
    NSString *path = [NSString stringWithCString:pathURL encoding:NSUTF8StringEncoding];
    // NSURL* directoryURL = [NSURL URLWithString:[directoryURLString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];

    if (imageBytes && imageSize > 0 && (FSPathMakeRef((const unsigned char *)[path fileSystemRepresentation], &par, &dir) == noErr))
    {

        NSData *tag = [NSData dataWithBytes:imageBytes length:imageSize];
        HFSUniStr255 fork = {0, {0}};
        sint16 refnum = kResFileNotOpened;
        FSGetResourceForkName(&fork);

        if (dir)
        {

            NSString *name = @"Icon\r";
            memset(&info, 0, sizeof(info));
            ((FileInfo*) (&info.finderInfo))->finderFlags = kIsInvisible;

            OSErr error = FSCreateResourceFile(&par, [name lengthOfBytesUsingEncoding:NSUTF16LittleEndianStringEncoding],
                    (UniChar*) [name cStringUsingEncoding:NSUTF16LittleEndianStringEncoding],
                    kFSCatInfoFinderXInfo, &info, fork.length, fork.unicode, &ref, NULL);

            if (error == dupFNErr)
            {
                // file already exists; prepare to try to open it
                const char *iconFileSystemPath =
//                        [[path stringByAppendingPathComponent:@"\000I\000c\000o\000n\000\r"] fileSystemRepresentation];
                        [[path stringByAppendingPathComponent:@"Icon\r"] fileSystemRepresentation];

                OSStatus status = FSPathMakeRef((const UInt8 *) iconFileSystemPath, &ref, NULL);
                if (status != noErr)
                {
                    fprintf(stderr, "error: FSPathMakeRef() returned %d for file \"%s\"\n", (int)status, iconFileSystemPath);

                }
            }
            else if (error != noErr)
            {
                return NO;
            }

        }
        else
        {
            // BlockMoveData(&par, &ref, sizeof(FSRef));
            memmove(&ref, &par, sizeof(FSRef));

            if (FSCreateResourceFork(&ref, fork.length, fork.unicode, 0) != noErr)
            {
                // test

                if (FSOpenResourceFile(&ref, fork.length, fork.unicode, fsRdWrPerm, (ResFileRefNum*) &refnum) != noErr)
                {
                    return NO;
                }

                if (refnum != kResFileNotOpened)
                {
                    UpdateResFile(refnum);
                    CloseResFile(refnum);

                    if (FSGetCatalogInfo(&par, kFSCatInfoFinderXInfo, &info, NULL, (FSSpecPtr)0, NULL) == noErr)
                    {
                        ((ExtendedFileInfo*) (&info.extFinderInfo))->extendedFinderFlags = kExtendedFlagsAreInvalid;
                        FSSetCatalogInfo(&par, kFSCatInfoFinderXInfo, &info);
                    }
                }

                // Test end
                return NO;
            }
        }

        OSErr errorr = FSOpenResourceFile(&ref, fork.length, fork.unicode, fsRdWrPerm, (ResFileRefNum*)&refnum);

        if (errorr != noErr)
        {
            return NO;
        }

        if (refnum != kResFileNotOpened)
        {
            CustomBadgeResource* cbr;

            int len = [tag length];
            Handle h = NewHandle(len);

            if (h)
            {
                // BlockMoveData([tag bytes], *h, len);
                memmove(*h, [tag bytes], len);

                AddResource(h, kIconFamilyType, 128, (const unsigned char *) "\\p");
                WriteResource(h);
                ReleaseResource(h);
            }

            h = NewHandle(sizeof(CustomBadgeResource));

            if (h)
            {
                cbr = (CustomBadgeResource*) *h;
                memset(cbr, 0, sizeof(CustomBadgeResource));
                cbr->version = kCustomBadgeResourceVersion;
                cbr->customBadgeResourceID = 128;
                AddResource(h, kCustomBadgeResourceType, kCustomBadgeResourceID, (const unsigned char *) "\\p");
                WriteResource(h);
                ReleaseResource(h);
            }

            UpdateResFile(refnum);
            CloseResFile(refnum);

            if (FSGetCatalogInfo(&par, kFSCatInfoFinderXInfo, &info, NULL, NULL, NULL) == noErr)
            {
                ((ExtendedFileInfo*) (&info.extFinderInfo))->extendedFinderFlags = kExtendedFlagHasCustomBadge;
                FSSetCatalogInfo(&par, kFSCatInfoFinderXInfo, &info);
            }
        }
    }

    return NO;
}

bool setBadgeIcon2(const char *path, char *imageBytes, int imageSize, char *fImageBytes, int fImageSize)
{
    bool didSetIcon = NO;
    NSWorkspace *ws = [NSWorkspace sharedWorkspace];
    NSFileManager *fm = [NSFileManager defaultManager];
    NSSize size = NSMakeSize(256, 256);

    // Directory path
    NSString *directoryURLString = [NSString stringWithCString:path encoding:NSUTF8StringEncoding];
    NSURL* directoryURL = [NSURL URLWithString:[directoryURLString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
    CFURLRef directoryURLRef =  (CFURLRef)directoryURL;

    // Badge image
    NSData *imageData = [NSData dataWithBytes:imageBytes length:imageSize];
    NSImage *badgeImage = resizeNSImage([[NSImage alloc] initWithData:imageData]);
    // [badgeImage setSize:size];

    NSData *fImageData = NULL;
    NSImage *fileIcon = NULL;

    if (fImageBytes != 0)
    {
        fImageData = [NSData dataWithBytes:fImageBytes length:fImageSize];
        fileIcon = [[NSImage alloc] initWithData:fImageData];
    }
    else
    {
        // Try to get file thumbnail image
        NSDictionary *options =
                [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:NO]
                forKey:(NSString *)kQLThumbnailOptionIconModeKey];

        CGImageRef fileCGImage = QLThumbnailImageCreate(
                    kCFAllocatorDefault, directoryURLRef, CGSizeMake(256, 256), (CFDictionaryRef) options);

        // If found
        if (NO) // fileCGImage != NULL)
        {
            fileIcon = [[NSImage alloc] initWithCGImage:fileCGImage size:size];
        }

        // If not found
        else
        {
            // get default icon
            fileIcon = [ws iconForFiles:[NSArray arrayWithObjects:directoryURLString, nil]];
        }

        // get default icon
        // fileIcon = [ws iconForFiles:[NSArray arrayWithObjects:directoryURLString, nil]];
    }

    if (fileIcon != NULL)
    {
        NSError *error = NULL;
        NSDictionary *fileAttributes = [fm attributesOfItemAtPath:directoryURLString error:&error];
        NSDate *date = [fileAttributes fileModificationDate];

        // Resize fileIcon
        /* [fileIcon setScalesWhenResized:YES];
        [[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationHigh];
        [fileIcon setSize:size]; */

        NSImage* iconImg = resizeNSImage(fileIcon);

        // Combine thumbnail with badge
        NSPoint aPoint = {0, 0};
        NSImage* resultImage = [[NSImage alloc] initWithSize:size];
        [resultImage lockFocus];

        [iconImg drawAtPoint:aPoint fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
        [badgeImage drawAtPoint:aPoint fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
        // Or any of the other about 6 options; see Apple's guide to pick.

        [resultImage unlockFocus];

        didSetIcon = [ws setIcon:resultImage forFile:[directoryURL path] options:0];

        // Restore modification date
        NSDictionary* attr = [NSDictionary dictionaryWithObjectsAndKeys: date, NSFileModificationDate, NULL];
        [[NSFileManager defaultManager] setAttributes: attr ofItemAtPath: directoryURLString error: &error];
    }

    return didSetIcon;
}

void showDockIcon(bool show)
{
    ProcessSerialNumber psn = {0, kCurrentProcess};

    if (show)
    {
        TransformProcessType(&psn, kProcessTransformToForegroundApplication);
    }
    else
    {
        TransformProcessType(&psn, kProcessTransformToUIElementApplication);
    }


    /* NSApplicationPresentationOptions options;

    if (show)
    {
        options = NSApplicationPresentationDefault;
    }
    else
    {
        options = NSApplicationPresentationHideDock + NSApplicationPresentationHideMenuBar;
    }

    @try
    {
        [NSApp setPresentationOptions:options];
    }
    @catch(NSException *exception)
    {
        NSLog(@"Error.  Make sure you have a valid combination of options.");
    } */
}
