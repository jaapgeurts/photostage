#ifndef MAPVIEW_OPENSTREETMAPMAPPROVIDER_H
#define MAPVIEW_OPENSTREETMAPMAPPROVIDER_H

#include <QNetworkReply>
#include <QQueue>

#include "mapprovider.h"
#include "previewcache.h"

namespace MapView
{
class OpenstreetmapMapProvider : public MapProvider
{
    Q_OBJECT

    public:

        OpenstreetmapMapProvider(QObject* parent = 0);
        ~OpenstreetmapMapProvider();

        // MapProvider interface

        void getTile(const QGeoCoordinate& coord, int zoomLevel);
        void getTiles(const QGeoCoordinate& topleft, int zoomLevel,
            int width,
            int height);

        QPoint coordToPixel(const QGeoCoordinate& coord, int zoomLevel) const;
        QGeoCoordinate pixelToCoord(const QPoint& point, int zoomLevel) const;

        QGeoCoordinate moveCoord(const QGeoCoordinate& coord,
            int dx,
            int dy,
            int zoomLevel) const;

        int minZoomLevel() const;
        int maxZoomLevel() const;

    private slots:

        void onReplyFinished(QNetworkReply* reply);

    private:

        const int     TILE_DIMENSION = 256;

        const QString TILESERVER_ENDPOINT1 =
            "http://otile1.mqcdn.com/tiles/1.0.0/osm";
        const QString TILESERVER_ENDPOINT2 =
            "http://otile2.mqcdn.com/tiles/1.0.0/osm";
        const QString TILESERVER_ENDPOINT3 =
            "http://otile3.mqcdn.com/tiles/1.0.0/osm";
        const QString TILESERVER_ENDPOINT4 =
            "http://otile4.mqcdn.com/tiles/1.0.0/osm";

        QNetworkAccessManager*   mNetworkManager;
        PhotoStage::PreviewCache mPreviewCache;
        bool                     mIsRunning;

        double long2tilex(double lon, int z) const;
        double lat2tiley(double lat, int z) const;
        double tilex2long(double x, int z) const;
        double tiley2lat(double y, int z) const;
        void fetchTile(Tile info);
        void startDownload(QQueue<Tile>& list);
        void cancelDownload();
};
}
#endif // OPENSTREETMAPMAPPROVIDER_H
