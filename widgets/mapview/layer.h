#ifndef MAPVIEW_LAYER_H
#define MAPVIEW_LAYER_H

#include <QObject>
#include <QPainter>

#include "mapview.h"
#include "abstractmarker.h"

namespace MapView
{


class Layer : public QObject
{
    Q_OBJECT

    public:

        explicit Layer(MapView* view);

        void addMarker(AbstractMarker* marker);

        void paint(QPainter* painter);

    signals:

    public slots:

        void clear();

    private:

        QList<AbstractMarker*> mMarkers;
        MapView*               mMapView;
};
}

#endif // LAYER_H
