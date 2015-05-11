#ifndef VOLUMEDISCOVERERIMPL_H
#define VOLUMEDISCOVERERIMPL_H

#include "volumediscoverer.h"
#include "volumediscovererplatform.h"

class VolumeDiscoverer::VolumeDiscovererImpl
{
public:
    VolumeDiscovererImpl();

    QStringList getPaths();

private:
    VolumeDiscovererPlatform *platformImpl;

};

#endif // VOLUMEDISCOVERERIMPL_H
