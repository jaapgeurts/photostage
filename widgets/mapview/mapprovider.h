#ifndef MAPVIEW_MAPPROVIDER_H
#define MAPVIEW_MAPPROVIDER_H

#include <QObject>
#include <QImage>
#include <QGeoCoordinate>
#include <QGeoRectangle>

namespace MapView
{
struct Tile
{
    int tilex, tiley;
    int canvas_x, canvas_y;
    int zoom;
    QImage image;
};

class MapProvider : public QObject
{
    Q_OBJECT

    public:

        explicit MapProvider(QObject* parent = 0);

        virtual void getTile(const QGeoCoordinate& coords, int zoomLevel) = 0;
        virtual void getTiles(const QGeoCoordinate& location, int zoomLevel,
            int width,
            int height) = 0;

        virtual QPoint coordToPixel(const QGeoCoordinate& coord,
            int zoomLevel) const = 0;
        virtual QGeoCoordinate pixelToCoord(const QPoint& point,
            int zoomLevel) const = 0;
        virtual QGeoCoordinate moveCoord(const QGeoCoordinate& coord,
            int dx,
            int dy,
            int zoomLevel) const = 0;

        virtual int getMinZoomLevel() const = 0;
        virtual int getMaxZoomLevel() const = 0;

    signals:

        void tileAvailable(const Tile& info);

    public slots:
};
}
#endif // MAPPROVIDER_H
