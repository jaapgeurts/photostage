#include <QDebug>

#include "layer.h"
#include "database/databaseaccess.h"

namespace MapView
{
MapView* Layer::mapView() const
{
    return mMapView;
}

Layer::Layer(MapView* view) :
    QObject(view)
{
    mMapView = view;
}

Layer::~Layer()
{
}
}
