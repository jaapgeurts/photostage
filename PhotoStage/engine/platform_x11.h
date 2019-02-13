#ifndef PLATFORM_X11_H
#define PLATFORM_X11_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

bool get_display_profile_path_x11(char* pathBuffer, size_t maxPathSize);

#ifdef __cplusplus
}
#endif


#endif // PLATFORM_X11_H
