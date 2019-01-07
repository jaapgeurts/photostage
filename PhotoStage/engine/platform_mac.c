#include <stdio.h>

#include "platform_mac.h"

static void printKeys (const void* key, const void* value, void* context)
{
    CFShow(key);
    CFShow(value);
}

// the pathBuffer will contain the path to the profile for the
// primary monitor
Boolean get_screen_profile(char* pathBuffer, size_t maxPathSize)
{
    CFUUIDRef displayUUID = CGDisplayCreateUUIDFromDisplayID(
        kCGDirectMainDisplay);


    if (CFGetTypeID(displayUUID) == CFNullGetTypeID())
    {
        printf("Cannot get display UUID.\n");
        return false;
    }

    CFDictionaryRef displayInfo = ColorSyncDeviceCopyDeviceInfo(
        kColorSyncDisplayDeviceClass,
        displayUUID);

    if (!displayInfo)
    {
        printf("Cannot get display info.\n");
        CFRelease(displayUUID);
        return false;
    }
    CFRelease(displayUUID);

    //  CFDictionaryApplyFunction(displayInfo,printKeys,NULL);

    CFDictionaryRef customProfileInfo = CFDictionaryGetValue(displayInfo,
            kColorSyncFactoryProfiles);// kColorSyncCustomProfiles);

    if (!customProfileInfo)
    {
        printf("Cannot get display profile info.\n");
        CFRelease(displayInfo);
        return false;
    }

    CFTypeRef* profileDict =
        (CFTypeRef*)malloc(CFDictionaryGetCount(customProfileInfo) *
            sizeof(CFTypeRef));
    CFDictionaryGetKeysAndValues(customProfileInfo,
        NULL,
        (const void**)profileDict);

    if ((void*)profileDict[0] == kCFNull)
    {
        printf("Cannot get display profile Dictionary.\n");
        CFRelease(displayInfo);
        free(profileDict);
        return false;
    }

   // CFDictionaryApplyFunction(profileDict[0], printKeys, NULL);

    CFTypeRef* profileDictValues = (CFTypeRef*)malloc(CFDictionaryGetCount(
                profileDict[0]) * sizeof(CFTypeRef));
    CFDictionaryGetKeysAndValues(profileDict[0],
        NULL,
        (const void**)profileDictValues);

    if ((void*)profileDictValues[0] == kCFNull)
    {
        printf("Cannot get display profile Url.\n");
        CFRelease(displayInfo);
        free(profileDict);
        free(profileDictValues);
        return false;
    }

    Boolean result = CFURLGetFileSystemRepresentation((CFURLRef)profileDictValues[0],
            true,
            (UInt8*)pathBuffer,
            maxPathSize);
    free(profileDict);
    free(profileDictValues);
    CFRelease(displayInfo);

    if (!result)
        printf("Cannot get display profile path.\n");

    return result;
}
