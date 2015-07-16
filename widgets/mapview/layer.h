#ifndef MAPVIEW_LAYER_H
#define MAPVIEW_LAYER_H

#include <QPainter>
#include <QAbstractItemModel>
#include <QItemSelectionModel>

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
            GeoCoordinateRole = Qt::UserRole + 3,
            CountRole         = Qt::UserRole + 4,
            DataRole          = Qt::UserRole + 5
        };

        explicit Layer(MapView* view);

        void setModel(QAbstractItemModel* model);
        void setSelectionModel(QItemSelectionModel* selectionModel);
        void setDelegate(AbstractMarker* delegate);

        void paint(QPainter& painter);

    signals:

        void markerDoubleClicked(const QModelIndex& index);

    public slots:

        void clear();

    private:

        QAbstractItemModel*  mItemModel;
        QItemSelectionModel* mSelectionModel;
        AbstractMarker*      mDelegate;
        MapView*             mMapView;
};
}

#endif // LAYER_H
