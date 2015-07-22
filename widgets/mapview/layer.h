#ifndef MAPVIEW_LAYER_H
#define MAPVIEW_LAYER_H

#include <QPainter>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QModelIndex>

#include "mapview.h"
#include "abstractmarker.h"

namespace MapView
{
class Layer : public QObject
{
    Q_OBJECT

    public:

        enum LayerRole
        {
            GeoCoordinateRole = Qt::UserRole + 200,
            CountRole         = Qt::UserRole + 201,
            DataRole          = Qt::UserRole + 202
        };

        explicit Layer(MapView* view);

        void setModel(QAbstractItemModel* model);
        void setSelectionModel(QItemSelectionModel* selectionModel);
        void setDelegate(AbstractMarker* delegate);

        void render(QPainter& painter);
        bool mousePressEvent(QMouseEvent *event);

        bool intersectsMarker(const QPoint& pos) const;
        QModelIndex indexAt(const QPoint& pos) const;


signals:

        void markerDoubleClicked(const QModelIndex& index);

    public slots:

        void viewChanged();

    private:

        QAbstractItemModel*  mItemModel;
        QItemSelectionModel* mSelectionModel;
        AbstractMarker*      mDelegate;
        MapView*             mMapView;
        QList<QModelIndex>   mCulled;

        QRect markerPosition(const QGeoCoordinate& coord) const;
};
}

#endif // LAYER_H
