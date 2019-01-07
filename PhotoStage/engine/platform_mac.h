#ifndef PLATFORM_MAC_H
#define PLATFORM_MAC_H

#include <ApplicationServices/ApplicationServices.h>

#ifdef __cplusplus
extern "C" {
#endif

Boolean get_screen_profile(char* pathBuffer, size_t maxPathSize);

#ifdef __cplusplus
}
#endif

#endif
