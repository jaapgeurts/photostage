#include <QDebug>

#include "layer.h"

namespace MapView
{
Layer::Layer(MapView* view) :
    QObject(view),
    mItemModel(NULL),
    mSelectionModel(NULL)
{
    mMapView = view;
}

void Layer::setModel(QAbstractItemModel* model)
{
    mItemModel = model;
    connect(mItemModel, &QAbstractItemModel::modelReset,
        this, &Layer::viewChanged);
    mCulled.clear();
}

void Layer::setSelectionModel(QItemSelectionModel* selectionModel)
{
    mSelectionModel = selectionModel;
}

void Layer::setDelegate(AbstractMarker* delegate)
{
    mDelegate = delegate;
}

void Layer::render(QPainter& painter)
{
    if (mItemModel == NULL)
    {
        qDebug() << "Model == NULL";
        // Nothing to do. return.
        return;
    }

    foreach(QModelIndex index, mCulled)
    {
        QGeoCoordinate coord =
            mItemModel->data(index,
                Layer::GeoCoordinateRole).value<QGeoCoordinate>();

        // prepare the viewport for the delegate
        QRect vp = markerPosition(coord);

        painter.save();
        painter.setClipRect(vp);
        painter.setWindow(QRect(0, 0, vp.width(), vp.height()));
        painter.setViewport(vp);

        MarkerInfo markerInfo;
        markerInfo.coord      = coord;
        markerInfo.modelIndex = index;
        markerInfo.x          = vp.left();
        markerInfo.y          = vp.top();
        markerInfo.width      = vp.width();
        markerInfo.height     = vp.height();

        if (mSelectionModel != NULL)
            markerInfo.markerState =
                mSelectionModel->selection().contains(index) ? MarkerInfo::
                MarkerStateSelected : MarkerInfo::MarkerStateNone;

        QVariant v = mItemModel->data(index, Layer::DataRole);
        mDelegate->paint(painter, markerInfo, v);
        painter.restore();
    }     // else skip, this coord is not in view
}

QRect Layer::markerPosition(const QGeoCoordinate& coord) const
{
    QPoint pos = mMapView->mapProvider()
        ->coordToPixel(coord, mMapView->zoomLevel());
    QPoint origin = mMapView->origin();

    QPoint absolute = pos - origin;

    QSize  size = mDelegate->size();
    QRect  vp;

    switch (mDelegate->anchor())
    {
        case AbstractMarker::TopLeft:
            vp.setCoords(absolute.x(), absolute.y(),
                absolute.x() + size.width(),
                absolute.y() + size.height());
            break;

        case AbstractMarker::TopRight:
            vp.setCoords(absolute.x() - size.width(), absolute.y(),
                absolute.x(), absolute.y() + size.height());
            break;

        case AbstractMarker::BottomRight:
            vp.setCoords(absolute.x() - size.width(),
                absolute.y() - size.height(), absolute.x(),
                absolute.y());
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
    return vp;
}

bool Layer::intersectsMarker(const QPoint& pos) const
{
    foreach(QModelIndex index, mCulled)
    {
        QGeoCoordinate coord = mItemModel->data(index,
                Layer::GeoCoordinateRole).value<QGeoCoordinate>();
        QRect          vp = markerPosition(coord);

        if (vp.contains(pos))
            return true;
    }
    return false;
}

QModelIndex Layer::indexAt(const QPoint& pos) const
{
    foreach(QModelIndex index, mCulled)
    {
        QGeoCoordinate coord = mItemModel->data(index,
                Layer::GeoCoordinateRole).value<QGeoCoordinate>();
        QRect          vp = markerPosition(coord);

        if (vp.contains(pos))
            return index;
    }
    return QModelIndex();
}

bool Layer::mousePressEvent(QMouseEvent* event)
{
    QModelIndex index;

    index = indexAt(event->pos());

    if (index.isValid())
    {
        mSelectionModel->setCurrentIndex(index,
            QItemSelectionModel::ClearAndSelect);
        return true;
    }
    return false;
}

void Layer::viewChanged()
{
    int           c        = mItemModel->rowCount(QModelIndex());
    QGeoRectangle viewPort = mMapView->mapBounds();

    // TODO: group markers together

    mCulled.clear();

    for (int i = 0; i < c; i++)
    {
        QModelIndex    index = mItemModel->index(i, 0);
        QGeoCoordinate coord =
            mItemModel->data(index,
                Layer::GeoCoordinateRole).value<QGeoCoordinate>();

        if (viewPort.contains(coord)) // skip markers not in view.
            mCulled.append(index);
    }

    //    foreach(QModelIndex index1, mCulled)
    //    {
    //        QGeoCoordinate coord1 =
    //            mItemModel->data(index1,
    //                Layer::GeoCoordinateRole).value<QGeoCoordinate>();
    //        QRect rect1 = markerPosition(coord);

    //         foreach(QModelIndex index2, mCulled)
    //         {
    //             if (index2 == index1)
    //                 continue;
    //             QGeoCoordinate coord2 =
    //                 mItemModel->data(index2,
    //                     Layer::GeoCoordinateRole).value<QGeoCoordinate>();
    //             QRect rect2 = markerPosition(coord);
    //             if (rect1.intersects(rect2))
    //                 continue;
    //         }
    //    }
}
}
