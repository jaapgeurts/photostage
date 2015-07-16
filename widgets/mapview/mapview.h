#ifndef MAPVIEW_MAPVIEW_H
#define MAPVIEW_MAPVIEW_H

#include <QWidget>
#include <QMouseEvent>
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

        QGeoRectangle mapBounds() const;

    public slots:

        void setZoomLevel(int level);

    signals:

    protected:

        void paintEvent(QPaintEvent*);
        void resizeEvent(QResizeEvent*);
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);
        void wheelEvent(QWheelEvent* event);

    private slots:

        void onTileAvailable(const Tile& info);
        void onZoomLevelChanged(int value);

    private:

        MapProvider*   mMapProvider;
        QList<Tile>    mTileList;
        QGeoCoordinate mOriginCoord, mCurrentCoord;
        QPoint         mOriginPixels;
        int            mZoomLevel;
        QRect          mTileBounds;
        QList<Layer*>  mLayers;
        QSlider*       mZoomSlider;
        bool           mPanning;
        QPoint         mMousePressLocation;
        QPoint         mDragDelta;

        void computeTileBounds();
        void fetchTiles();
        void moveOrigin(const QPoint& delta);
};
}

#endif // MAPVIEW_H