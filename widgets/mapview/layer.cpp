#include "layer.h"

namespace MapView
{
Layer::Layer(QObject* parent) : QObject(parent)
{
}

void Layer::addMarker(AbstractMarker* marker)
{
    mMarkers.append(marker);
}

void Layer::clear()
{
    qDeleteAll(mMarkers);
    mMarkers.clear();
}
}
