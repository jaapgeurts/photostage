#ifndef MAPVIEW_LAYER_H
#define MAPVIEW_LAYER_H

#include <QObject>

#include "abstractmarker.h"

namespace MapView
{
class Layer : public QObject
{
    Q_OBJECT

    public:

        explicit Layer(QObject* parent = 0);

        void addMarker(AbstractMarker* marker);

    signals:

    public slots:

        void clear();

    private:

        QList<AbstractMarker*> mMarkers;
};
}

#endif // LAYER_H
