#ifndef OPENSTREETMAPMAPPROVIDER_H
#define OPENSTREETMAPMAPPROVIDER_H

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
        void getTiles(const QGeoCoordinate& center,
            int zoomLevel,
            int width,
            int height);

    private slots:

        void onReplyFinished(QNetworkReply* reply);

    private:

        const QString TILESERVER_ENDPOINT1 =
            "http://otile1.mqcdn.com/tiles/1.0.0/osm";
        const QString TILESERVER_ENDPOINT2 =
            "http://otile2.mqcdn.com/tiles/1.0.0/osm";
        const QString TILESERVER_ENDPOINT3 =
            "http://otile3.mqcdn.com/tiles/1.0.0/osm";
        const QString TILESERVER_ENDPOINT4 =
            "http://otile4.mqcdn.com/tiles/1.0.0/osm";

        QNetworkAccessManager* mNetworkManager;
        PhotoStage::PreviewCache           mPreviewCache;
        bool                   mIsRunning;

        double long2tilex(double lon, int z);
        double lat2tiley(double lat, int z);
        double tilex2long(int x, int z);
        double tiley2lat(int y, int z);
        void fetchTile(TileInfo info);
        void startDownload(QQueue<TileInfo> &list);
        void cancelDownload();
};
}
#endif // OPENSTREETMAPMAPPROVIDER_H
