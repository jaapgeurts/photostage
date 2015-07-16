#include <QDebug>

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
    QList<AbstractMarker*> culled;

    QGeoRectangle          rect = mMapView->mapBounds();

    foreach(AbstractMarker * m, mMarkers)
    {
        if (rect.contains(m->coord()))
        {
            qDebug() << "Append a marker";
            culled.append(m);
        }
    }
    qDebug() << "marker list size" << culled.size();
    /*
            //find all markers close to each other
            QList<AbstractMarker*> grouped;
            foreach(AbstractMarker* m1, culled)
            {
                foreach(AbstractMarker*m2, culled)
                {
                    if (m1->coord().distanceTo(m2) /
            }
     */
    foreach(AbstractMarker * m, culled)
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
