#include <QDebug>

#include "modelindexlayer.h"

#include "mapview.h"

namespace MapView
{
ModelIndexLayer::ModelIndexLayer(MapView* view)
    : Layer(view),
    mItemModel(NULL),
    mSelectionModel(NULL)
{
}

void ModelIndexLayer::setModel(QAbstractItemModel* model)
{
    mItemModel = model;
    connect(mItemModel, &QAbstractItemModel::modelReset, this, &ModelIndexLayer::viewChanged);
    mCulled.clear();

    connect(mItemModel, &QAbstractItemModel::dataChanged, this, &ModelIndexLayer::onDataChanged);
    connect(mItemModel, &QAbstractItemModel::rowsInserted, this, &ModelIndexLayer::onRowsAdded);
    connect(mItemModel, &QAbstractItemModel::rowsRemoved, this, &ModelIndexLayer::onRowsRemoved);
}

void ModelIndexLayer::setSelectionModel(QItemSelectionModel* selectionModel)
{
    mSelectionModel = selectionModel;
}

void ModelIndexLayer::setDelegate(AbstractMarker* delegate)
{
    mDelegate = delegate;
}

void ModelIndexLayer::render(QPainter& painter)
{
    if (mItemModel == NULL)
    {
        qDebug() << "Model == NULL";
        // Nothing to do. return.
        return;
    }

    foreach(QModelIndex index, mCulled)
    {
        QGeoCoordinate coord = mItemModel->data(index, ModelIndexLayer::GeoCoordinateRole).value<QGeoCoordinate>();

        if (!coord.isValid())
            continue;

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
            markerInfo.markerState = mSelectionModel->selection().contains(index)
                ? MarkerInfo::MarkerStateSelected : MarkerInfo::MarkerStateNone;

        QVariant v = mItemModel->data(index, ModelIndexLayer::DataRole);
        mDelegate->paint(painter, markerInfo, v);
        painter.restore();
    }     // else skip, this coord is not in view
}

QRect ModelIndexLayer::markerPosition(const QGeoCoordinate& coord) const
{
    QPoint pos    = mapView()->mapProvider()->coordToPixel(coord, mapView()->zoomLevel());
    QPoint origin = mapView()->origin();

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

bool ModelIndexLayer::intersectsMarker(const QPoint& pos) const
{
    foreach(QModelIndex index, mCulled)
    {
        QGeoCoordinate coord = mItemModel->data(index, ModelIndexLayer::GeoCoordinateRole).value<QGeoCoordinate>();

        if (!coord.isValid())
            continue;

        QRect vp = markerPosition(coord);

        if (vp.contains(pos))
            return true;
    }
    return false;
}

QModelIndex ModelIndexLayer::indexAt(const QPoint& pos) const
{
    foreach(QModelIndex index, mCulled)
    {
        QGeoCoordinate coord = mItemModel->data(index, ModelIndexLayer::GeoCoordinateRole).value<QGeoCoordinate>();

        if (!coord.isValid())
            continue;

        QRect vp = markerPosition(coord);

        if (vp.contains(pos))
            return index;
    }
    return QModelIndex();
}

bool ModelIndexLayer::mousePressEvent(QMouseEvent* event)
{
    QModelIndex index;

    index = indexAt(event->pos());

    if (index.isValid())
    {
        mSelectionModel->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
        return true;
    }
    return false;
}

bool ModelIndexLayer::mouseMoveEvent(QMouseEvent* event)
{
    bool intersects = intersectsMarker(event->pos());

    if (intersects)
        mapView()->setCursor(QCursor(Qt::ArrowCursor));
    else
        mapView()->setCursor(QCursor(Qt::OpenHandCursor));

    return !intersects;
}

void ModelIndexLayer::viewChanged()
{
    int           c        = mItemModel->rowCount(QModelIndex());
    QGeoRectangle viewPort = mapView()->mapBounds();

    // TODO: group markers together

    mCulled.clear();

    for (int i = 0; i < c; i++)
    {
        QModelIndex    index = mItemModel->index(i, 0);

        QGeoCoordinate coord = mItemModel->data(index, ModelIndexLayer::GeoCoordinateRole).value<QGeoCoordinate>();

        if (!coord.isValid())
            continue;

        if (viewPort.contains(coord)) // skip markers not in view.
            mCulled.append(index);
    }

    //    foreach(QModelIndex index1, mCulled)
    //    {
    //        QGeoCoordinate coord1 =
    //            mItemModel->data(index1,
    //                ModelIndexLayer::GeoCoordinateRole).value<QGeoCoordinate>();
    //        QRect rect1 = markerPosition(coord);

    //         foreach(QModelIndex index2, mCulled)
    //         {
    //             if (index2 == index1)
    //                 continue;
    //             QGeoCoordinate coord2 =
    //                 mItemModel->data(index2,
    //                     ModelIndexLayer::GeoCoordinateRole).value<QGeoCoordinate>();
    //             QRect rect2 = markerPosition(coord);
    //             if (rect1.intersects(rect2))
    //                 continue;
    //         }
    //    }
}

void ModelIndexLayer::onRowsAdded(const QModelIndex& parent, int first, int last)
{
    qDebug () << "rows added";
}

void ModelIndexLayer::onRowsRemoved(const QModelIndex& parent, int first, int last)
{
    qDebug () << "rows removed";
}

void ModelIndexLayer::onDataChanged(const QModelIndex& topleft,
    const QModelIndex& bottomright,
    const QVector<int>& roles)
{
    viewChanged();
}
}
