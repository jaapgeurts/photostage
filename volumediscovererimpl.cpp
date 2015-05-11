#include "volumediscovererimpl.h"
#include "volumediscoverermac.h"

#include <QtGlobal>

#ifdef Q_WS_MAC
#include "volumediscoverermac.h"
#endif

VolumeDiscoverer::VolumeDiscovererImpl::VolumeDiscovererImpl()
{
#ifdef Q_WS_MAC
  platformImpl = new VolumeDiscovererMac();
#endif
}


QStringList VolumeDiscoverer::VolumeDiscovererImpl::getPaths()
{
    return platformImpl->getPaths();
}
