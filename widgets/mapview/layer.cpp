#include "layer.h"

namespace MapView
{
Layer::Layer(MapView* view) : QObject(view)
{
    mMapView = view;
}

void Layer::addMarker(AbstractMarker* marker)
{
    mMarkers.append(marker);
}

void Layer::paint(QPainter* painter)
{
    foreach(AbstractMarker * m, mMarkers)
    {
        QGeoCoordinate coord = m->coord();
        QPoint         pos   = mMapView->mapProvider()
            ->coordToPixel(coord, mMapView->zoomLevel());
        QPoint         origin = mMapView->origin();

        QPoint         abso = pos - origin;

        QRect          bounds = m->bounds();
        QRect          vp;

        switch (m->anchor())
        {
            case AbstractMarker::TopLeft:
                vp.setCoords(abso.x(), abso.y(),
                    abso.x() + bounds.width(), abso.y() + bounds.height());
                break;

            case AbstractMarker::TopRight:
                vp.setCoords(abso.x() - bounds.width(), abso.y(),
                    abso.x(), abso.y() + bounds.height());
                break;

            case AbstractMarker::BottomRight:
                vp.setCoords(abso.x() - bounds.width(),
                    abso.y() - bounds.height(), abso.x(), abso.y());
                break;

            case AbstractMarker::BottomLeft:
                vp.setCoords(abso.x(), abso.y() - bounds.height(),
                    abso.x() + bounds.width(), abso.y());
                break;

            case AbstractMarker::CenterLeft:
                vp.setCoords(abso.x(), abso.y() - bounds.height() / 2,
                    abso.x() + bounds.width(), abso.y() + bounds.height() / 2);
                break;

            case AbstractMarker::CenterRight:
                vp.setCoords(abso.x() - bounds.width(),
                    abso.y() - bounds.height() / 2,
                    abso.x(), abso.y() + bounds.height() / 2);
                break;

            case AbstractMarker::CenterTop:
                vp.setCoords(abso.x() - bounds.width() / 2, abso.y(),
                    abso.x() + bounds.width() / 2, abso.y() + bounds.height());
                break;

            case AbstractMarker::CenterBottom:
                vp.setCoords(abso.x() - bounds.width() / 2,
                    abso.y() - bounds.height(),
                    abso.x() + bounds.width() / 2, abso.y());
                break;

            case AbstractMarker::Center:
                vp.setCoords(abso.x() - bounds.width() / 2,
                    abso.y() - bounds.height() / 2,
                    abso.x() + bounds.width() / 2,
                    abso.y() + bounds.height() / 2);
                break;
        }

        painter->save();
        painter->setClipRect(vp);
        painter->setWindow(bounds);
        painter->setViewport(vp);

        m->paint(painter);
        painter->restore();
    }
}

void Layer::clear()
{
    qDeleteAll(mMarkers);
    mMarkers.clear();
}
}
