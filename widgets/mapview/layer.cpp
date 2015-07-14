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
    // cull markers not in view and group markers that are too close
    //    QList<AbstractMarker *> list;

    //    foreach(AbstractMarker * m, mMarkers)
    //    {

    //    }

    foreach(AbstractMarker * m, mMarkers)
    {
        QGeoCoordinate coord = m->coord();
        QPoint         pos   = mMapView->mapProvider()
            ->coordToPixel(coord, mMapView->zoomLevel());
        QPoint         origin = mMapView->origin();

        QPoint         absolute = pos - origin;

        QSize          size = m->size();
        QRect          vp;

        switch (m->anchor())
        {
            case AbstractMarker::TopLeft:
                vp.setCoords(absolute.x(), absolute.y(),
                    absolute.x() + size.width(), absolute.y() + size.height());
                break;

            case AbstractMarker::TopRight:
                vp.setCoords(absolute.x() - size.width(), absolute.y(),
                    absolute.x(), absolute.y() + size.height());
                break;

            case AbstractMarker::BottomRight:
                vp.setCoords(absolute.x() - size.width(),
                    absolute.y() - size.height(), absolute.x(), absolute.y());
                break;

            case AbstractMarker::BottomLeft:
                vp.setCoords(absolute.x(), absolute.y() - size.height(),
                    absolute.x() + size.width(), absolute.y());
                break;

            case AbstractMarker::CenterLeft:
                vp.setCoords(absolute.x(), absolute.y() - size.height() / 2,
                    absolute.x() + size.width(),
                    absolute.y() + size.height() / 2);
                break;

            case AbstractMarker::CenterRight:
                vp.setCoords(absolute.x() - size.width(),
                    absolute.y() - size.height() / 2,
                    absolute.x(), absolute.y() + size.height() / 2);
                break;

            case AbstractMarker::CenterTop:
                vp.setCoords(absolute.x() - size.width() / 2, absolute.y(),
                    absolute.x() + size.width() / 2,
                    absolute.y() + size.height());
                break;

            case AbstractMarker::CenterBottom:
                vp.setCoords(absolute.x() - size.width() / 2,
                    absolute.y() - size.height(),
                    absolute.x() + size.width() / 2, absolute.y());
                break;

            case AbstractMarker::Center:
                vp.setCoords(absolute.x() - size.width() / 2,
                    absolute.y() - size.height() / 2,
                    absolute.x() + size.width() / 2,
                    absolute.y() + size.height() / 2);
                break;
        }

        painter->save();
        painter->setClipRect(vp);
        painter->setWindow(QRect(0, 0, size.width(), size.height()));
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
