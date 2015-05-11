#include "volumediscoverer.h"
#include "volumediscovererimpl.h"

VolumeDiscoverer::VolumeDiscoverer()
{
    vd = new VolumeDiscovererImpl();
}

VolumeDiscoverer::~VolumeDiscoverer()
{
    delete vd;
}

QStringList VolumeDiscoverer::getPaths()
{
    return vd->getPaths();
}


