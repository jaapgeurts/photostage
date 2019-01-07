#ifndef MAPVIEW_LAYER_H
#define MAPVIEW_LAYER_H

#include <QPainter>
#include <QMouseEvent>

#include "mapview.h"
#include "abstractmarker.h"

namespace MapView
{
class Layer : public QObject
{
    Q_OBJECT

    public:

        virtual ~Layer() = 0;

        virtual void render(QPainter& painter)           = 0;
        virtual void viewChanged()                       = 0;
        virtual bool mousePressEvent(QMouseEvent* event) = 0;
        virtual bool mouseMoveEvent(QMouseEvent* event)  = 0;

        MapView* mapView() const;

    protected:

        explicit Layer(MapView* view);

    private:

        MapView* mMapView;
};
}

#endif // LAYER_H
