#ifndef VOLUMEDISCOVERER_H
#define VOLUMEDISCOVERER_H

#include <QStringList>


class VolumeDiscoverer
{

public:

    VolumeDiscoverer();
    ~VolumeDiscoverer();

    QStringList getPaths();

private:
    class VolumeDiscovererImpl;
    VolumeDiscovererImpl* vd;

};

#endif // VOLUMEDISCOVERER_H
