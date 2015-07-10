#ifndef MAPPROVIDER_H
#define MAPPROVIDER_H

#include <QObject>
#include <QImage>
#include <QGeoCoordinate>
#include <QGeoRectangle>

namespace PhotoStage
{
struct MapTileInfo
{
    public:

        int    tilex, tiley;
        int    canvas_x, canvas_y;
        int    zoom;
        QImage image;
};

class MapProvider : public QObject
{
    Q_OBJECT

    public:

        explicit MapProvider(QObject* parent = 0);

        virtual void getTile(const QGeoCoordinate& coords,
            int zoomLevel) = 0;
        virtual void getTiles(const QGeoCoordinate& location,
            int zoomLevel,
            int width,
            int height) = 0;

    signals:

        void tileAvailable(const MapTileInfo& info);

    public slots:
};
}
#endif // MAPPROVIDER_H
