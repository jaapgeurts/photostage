
#import <AppKit/AppKit.h>

#include "volumediscoverermac.h"


QStringList VolumeDiscovererMac::getPaths()
{
    NSWorkspace * ws = [NSWorkspace sharedWorkspace];
    NSArray* volumes = [ws mountedLocalVolumePaths];
    NSFileManager *fm = [NSFileManager defaultManager];

    for (NSString *path in volumes)
    {
      NSDictionary *fsAttributes;
      NSString * description, *type, *name;
      NSError *error;
      BOOL removable, writable, unmountable, res;
      unsigned long long size;

      res = [ws getFileSystemInfoForPath:path
                                 isRemovable:&removable
                                  isWritable:&writable
                               isUnmountable:&unmountable
                                 description:&description
                                        type:&type];
          if (!res)
              continue;

              name         = [fm displayNameAtPath:path];
              fsAttributes = [[NSFileManager defaultManager] attributesOfFileSystemForPath:@"/" error:&error];
              size = [[fsAttributes objectForKey:NSFileSystemFreeSize] longLongValue];

              NSLog(@"path=%@\nname=%@\nremovable=%d\nwritable=%d\nunmountable=%d\n"
                     "description=%@\ntype=%@, size=%qu\n\n",
                    path, name, removable, writable, unmountable, description, type, size);

    }

    return QStringList();
}




