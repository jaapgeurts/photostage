#include <stdio.h>


#include "platform_x11.h"


// the pathBuffer will contain the path to the profile for the
// primary monitor
bool get_display_profile_path_x11(char* pathBuffer, size_t maxPathSize)
{
  fprintf(stderr,"Color profiles not implemented\n");
  return false;
  /*
  // open default display as defined by $DISPLAY.
  Display *dpy = XOpenDisplay(NULL);
  if (dpy == NULL) {
    char* env_display = getenv("DISPLAY");
    if (env_display != NULL)
      fprintf(stderr,"Can't open display: %s\n",env_display);
    else
      fprintf(stderr,"Can't open display because DISPLAY variable is not defined\n");
    return false;
  }

  //Atom name: "_ICC_DEVICE_PROFILE(_xxx)"
  prop_desktop = XInternAtom(dpy, "_ICC_DEVICE_PROFILE", true);

  // assume everything here is already defined
  int status = XGetWindowProperty(XDPY, child, prop_desktop,
                                  0L, 1L, False,
                                  AnyPropertyType, &type, &format,
                                  &length, &after, &data);

  if (status == Success and type != None)
    printf("Atom Property Value: %s \n", (const char *) data);
  return false;
  */
}
