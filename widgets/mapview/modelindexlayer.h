#ifndef MAPVIEW_MODELINDEXLAYER_H
#define MAPVIEW_MODELINDEXLAYER_H

#include <QPainter>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QModelIndex>

#include "layer.h"

namespace MapView
{
class ModelIndexLayer : public Layer
{
    Q_OBJECT

    public:

        enum LayerRole
        {
            GeoCoordinateRole = Qt::UserRole + 200,
            DataRole          = Qt::UserRole
        };

        explicit ModelIndexLayer(MapView* view);

        void setModel(QAbstractItemModel* model);
        void setSelectionModel(QItemSelectionModel* selectionModel);
        void setDelegate(AbstractMarker* delegate);

        void render(QPainter& painter);
        bool mousePressEvent(QMouseEvent* event);
        bool mouseMoveEvent(QMouseEvent* event);

        bool intersectsMarker(const QPoint& pos) const;
        QModelIndex indexAt(const QPoint& pos) const;

    signals:

        void markerDoubleClicked(const QModelIndex& index);

    public slots:

        void viewChanged();

    private slots:

        void onRowsAdded(const QModelIndex& parent, int first, int last);
        void onRowsRemoved(const QModelIndex& parent, int first, int last);
        void onDataChanged(const QModelIndex& topleft, const QModelIndex& bottomright, const QVector<int>& roles);

    private:

        QAbstractItemModel*  mItemModel;
        QItemSelectionModel* mSelectionModel;
        AbstractMarker*      mDelegate;
        QList<QModelIndex>   mCulled;

        QRect markerPosition(const QGeoCoordinate& coord) const;
};
}
#endif // MAPVIEW_MODELINDEXLAYER_H
