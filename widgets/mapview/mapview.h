#ifndef MAPVIEW_MAPVIEW_H
#define MAPVIEW_MAPVIEW_H

#include <QObject>
#include <QWidget>
#include <QGeoCoordinate>
#include <QList>

#include "mapprovider.h"
#include "layer.h"

namespace MapView
{
class MapView : public QWidget
{
    Q_OBJECT

    public:

        explicit MapView(QWidget* parent = 0);
        virtual ~MapView();

        QSize sizeHint() const;
        void setCurrentCoord(const QGeoCoordinate& coord);

        void addLayer(Layer* layer);

    signals:

    public slots:

        void setMapProvider(MapProvider* provider);

    protected:

        void paintEvent(QPaintEvent*);
        void resizeEvent(QResizeEvent*);

    private slots:

        void onTileAvailable(const Tile& info);

    private:

        MapProvider*   mMapProvider;
        QList<Tile>    mTileInfoList;
        QGeoCoordinate mCurrentCoord;
        QImage         mIconMapPin;
        int            mZoomLevel;
        QRect          mTileBounds;
        QList<Layer*>   mLayers;

        void computeTileBounds();
};
}

#endif // MAPVIEW_H
