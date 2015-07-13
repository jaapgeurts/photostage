#ifndef MAPVIEW_MAPVIEW_H
#define MAPVIEW_MAPVIEW_H

#include <QObject>
#include <QWidget>
#include <QGeoCoordinate>
#include <QList>
#include <QSlider>

#include "mapprovider.h"

namespace MapView
{
class Layer;

class MapView : public QWidget
{
    Q_OBJECT

    public:

        explicit MapView(QWidget* parent = 0);
        virtual ~MapView();

        QSize sizeHint() const;
        void setCurrentCoord(const QGeoCoordinate& coord);

        void addLayer(Layer* layer);

        MapProvider* mapProvider() const;
        void setMapProvider(MapProvider* provider);

        int zoomLevel() const;
        QPoint origin() const;

    public slots:

        void setZoomLevel(int level);

    signals:

    protected:

        void paintEvent(QPaintEvent*);
        void resizeEvent(QResizeEvent*);

    private slots:

        void onTileAvailable(const Tile& info);
        void onZoomLevelChanged(int value);

    private:

        MapProvider*   mMapProvider;
        QList<Tile>    mTileList;
        QGeoCoordinate mCurrentCoord;
        QPoint         mOrigin;
        int            mZoomLevel;
        QRect          mTileBounds;
        QList<Layer*>  mLayers;
        QSlider*       mZoomSlider;

        void fetchTiles();
        void computeTileBounds();
};
}

#endif // MAPVIEW_H
