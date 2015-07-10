#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QGeoRectangle>
#include <QDir>
#include <QList>

#include <math.h>

#include "openstreetmapmapprovider.h"

namespace PhotoStage
{
class TileInfoWrapper : public QObjectUserData
{
    public:

        MapTileInfo info;
        QString     key;
};

OpenstreetmapMapProvider::OpenstreetmapMapProvider(QObject* parent) :
    MapProvider(parent),
    mPreviewCache(
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
        + QDir::separator() + "osmtiles", NULL),
    mIsRunning(false)
{
    mNetworkManager = new QNetworkAccessManager(this);

    connect(mNetworkManager, &QNetworkAccessManager::finished,
        this, &OpenstreetmapMapProvider::onReplyFinished);
}

OpenstreetmapMapProvider::~OpenstreetmapMapProvider()
{
}

void OpenstreetmapMapProvider::getTiles(const QGeoCoordinate& center,
    int zoomLevel,
    int width,
    int height)
{
    cancelDownload();

    double tilex_exact = long2tilex(center.longitude(), zoomLevel);
    double tiley_exact = lat2tiley(center.latitude(), zoomLevel);

    double tilex_offset = tilex_exact - (int64_t)tilex_exact;
    double tiley_offset = tiley_exact - (int64_t)tiley_exact;
    qDebug() << "Fract" << tilex_offset << "," << tiley_offset;

    //    tilex_offset = (width/2) % 256 - tilex_offset * 256;
    //    tiley_offset = (height/2) % 256 - tiley_offset * 256;

    // these coords are the center tile
    int tilex = (int)floor(tilex_exact);
    int tiley = (int)floor(tiley_exact);

    // our tiles are 256 pixels so we need width/256 and height/256 tiles (rounded up)
    // add one more depending on where the center coord is located
    int no_x = width / 256 + 1; // get one more than needed
    int no_y = height / 256 + 1;

    int x_start = tilex - no_x / 2;
    int x_end   = tilex + no_x / 2 + no_x % 2;
    int y_start = tiley - no_y / 2;
    int y_end   = tiley + no_y / 2 + no_y % 2;

    //    mTileResult.bounds.setTopLeft(QGeoCoordinate(tiley2lat(tiley, zoomLevel),
    //        tilex2long(tilex, zoomLevel)));
    //    mTileResult.bounds.setBottomRight(QGeoCoordinate(tiley2lat(tiley + 1,
    //        zoomLevel), tilex2long(tilex + 1, zoomLevel)));

    QQueue<MapTileInfo> list;

    for (int y = y_start; y < y_end; y++)
    {
        for (int x = x_start; x < x_end; x++)
        {
            MapTileInfo info;
            info.tilex    = x;
            info.tiley    = y;
            info.canvas_x = (x - x_start) * 256 - width/2 % 256;// - tilex_offset * 256; //
            info.canvas_y = (y - y_start) * 256 - height/2 % 256;// - tiley_offset * 256;//
            info.zoom = zoomLevel;

            list.enqueue(info);
        }
    }
    startDownload(list);
}

void OpenstreetmapMapProvider::getTile(const QGeoCoordinate& coord,
    int zoomLevel)
{
    cancelDownload();

    int tilex = long2tilex(coord.longitude(), zoomLevel);
    int tiley = lat2tiley(coord.latitude(), zoomLevel);

    //    mTileResult.bounds.setTopLeft(QGeoCoordinate(tiley2lat(tiley, zoomLevel),
    //        tilex2long(tilex, zoomLevel)));
    //    mTileResult.bounds.setBottomRight(QGeoCoordinate(tiley2lat(tiley + 1,
    //        zoomLevel),
    //        tilex2long(tilex + 1, zoomLevel)));

    MapTileInfo info;
    info.tilex    = tilex;
    info.tiley    = tiley;
    info.canvas_x = 0;
    info.canvas_y = 0;
    info.zoom     = zoomLevel;

    QQueue<MapTileInfo> queue;
    queue.enqueue(info);

    startDownload(queue);
}

void OpenstreetmapMapProvider::startDownload(QQueue<MapTileInfo>& queue)
{
    mIsRunning = true;

    while (queue.size() > 0)
    {
        MapTileInfo info = queue.dequeue();
        fetchTile(info);
    }
}

void OpenstreetmapMapProvider::cancelDownload()
{
    // TODO protect by mutex
    mIsRunning = false;
}

void OpenstreetmapMapProvider::fetchTile(MapTileInfo info)
{
    QString url = QString("%1/%2/%3/%4.png").arg(TILESERVER_ENDPOINT1)
        .arg(info.zoom).arg(info.tilex).arg(info.tiley);
    QString key = QString("%2/%3/%4.png").arg(info.zoom)
        .arg(info.tilex).arg(info.tiley);

    QImage img = mPreviewCache.get(key);


    if (img.isNull())
    {
        QNetworkRequest request;

        request.setUrl(QUrl(url));
        request.setRawHeader("User-Agent", "PhotoStage Map Service Agent 1.0");
        TileInfoWrapper* wrapper = new TileInfoWrapper();
        wrapper->info = info;
        wrapper->key  = key;
        QNetworkReply* reply = mNetworkManager->get(request);
        reply->setProperty("wrapper", qVariantFromValue((void*)wrapper));
    }
    else
    {
        info.image = img;
        emit tileAvailable(info);
    }
}

void OpenstreetmapMapProvider::onReplyFinished(QNetworkReply* reply)
{
    TileInfoWrapper* wrapper =
        (TileInfoWrapper*)reply->property("wrapper").value<void*>();


    reply->setProperty("wrapper", QVariant());

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error:" << reply->errorString();
        reply->deleteLater();
        delete wrapper;
        return;
    }

    QImage img = QImage::fromData(reply->readAll());

    mPreviewCache.put(wrapper->key, img);
    wrapper->info.image = img;

    reply->deleteLater();

    emit tileAvailable(wrapper->info);
    delete wrapper;
}

double OpenstreetmapMapProvider::long2tilex(double lon, int z)
{
    return (lon + 180.0) / 360.0 * pow(2.0, z);
}

double OpenstreetmapMapProvider::lat2tiley(double lat, int z)
{
    return (1.0 - log( tan(lat * M_PI / 180.0) + 1.0 /
           cos(lat * M_PI / 180.0)) / M_PI) / 2.0 * pow(2.0, z);
}

double OpenstreetmapMapProvider::tilex2long(int x, int z)
{
    return x / pow(2.0, z) * 360.0 - 180;
}

double OpenstreetmapMapProvider::tiley2lat(int y, int z)
{
    double n = M_PI - 2.0 * M_PI * y / pow(2.0, z);


    return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}
}
