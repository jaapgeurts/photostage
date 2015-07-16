#ifndef MAPVIEW_LAYER_H
#define MAPVIEW_LAYER_H

#include <QPainter>
#include <QAbstractItemModel>

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
            CountRole         = Qt::UserRole + 4
        };

        explicit Layer(MapView* view);

        void setModel(QAbstractItemModel* model);

        void paint(QPainter* painter);

    signals:

    public slots:

        void clear();

    private:

        QAbstractItemModel* mPhotoModel;
        MapView*            mMapView;
};
}

#endif // LAYER_H
