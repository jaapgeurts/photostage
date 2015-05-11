#ifndef VOLUMEDISCOVERERPLATFORM_H
#define VOLUMEDISCOVERERPLATFORM_H

#include <QStringList>

class VolumeDiscovererPlatform
{
public:

     virtual QStringList getPaths() = 0;
};

#endif // VOLUMEDISCOVERERPLATFORM_H
