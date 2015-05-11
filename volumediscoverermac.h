#ifndef VOLUMEDISCOVERERMAC_H
#define VOLUMEDISCOVERERMAC_H

#include "volumediscovererplatform.h"

class VolumeDiscovererMac : public VolumeDiscovererPlatform
{
public:

   QStringList getPaths();
};

#endif // VOLUMEDISCOVERERMAC_H

