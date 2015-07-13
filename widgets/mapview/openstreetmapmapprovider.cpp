#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QGeoRectangle>
#include <QDir>
#include <QList>

#include <math.h>

#include "openstreetmapmapprovider.h"

namespace MapView
{
class TileInfoWrapper : public QObjectUserData
{
    public:

        Tile    info;
        QString key;
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

void OpenstreetmapMapProvider::getTile(const QGeoCoordinate& coord,
    int zoomLevel)
{
    cancelDownload();

    double tilex_exact = long2tilex(coord.longitude(), zoomLevel);
    double tiley_exact = lat2tiley(coord.latitude(), zoomLevel);

    //    mTileResult.bounds.setTopLeft(QGeoCoordinate(tiley2lat(tiley, zoomLevel),
    //        tilex2long(tilex, zoomLevel)));
    //    mTileResult.bounds.setBottomRight(QGeoCoordinate(tiley2lat(tiley + 1,
    //        zoomLevel),
    //        tilex2long(tilex + 1, zoomLevel)));

    Tile info;
    info.x    = tilex_exact;
    info.y    = tiley_exact;
    info.zoom = zoomLevel;

    QQueue<Tile> queue;
    queue.enqueue(info);

    startDownload(queue);
}

void OpenstreetmapMapProvider::getTiles(const QGeoCoordinate& topleft,
    int zoomLevel,
    int width,
    int height)
{
    cancelDownload();

    double tilex_exact = long2tilex(topleft.longitude(), zoomLevel);
    double tiley_exact = lat2tiley(topleft.latitude(), zoomLevel);

    //    double tilex_offset = tilex_exact - (int64_t)tilex_exact;
    //    double tiley_offset = tiley_exact - (int64_t)tiley_exact;

    int x_start = (int)floor(tilex_exact);
    int y_start = (int)floor(tiley_exact);

    int x_end = (int)floor(tilex_exact) + width / TILE_DIMENSION + 1;
    int y_end = (int)floor(tiley_exact) + height / TILE_DIMENSION + 1;

    //    mTileResult.bounds.setTopLeft(QGeoCoordinate(tiley2lat(tiley, zoomLevel),
    //        tilex2long(tilex, zoomLevel)));
    //    mTileResult.bounds.setBottomRight(QGeoCoordinate(tiley2lat(tiley + 1,
    //        zoomLevel), tilex2long(tilex + 1, zoomLevel)));

    QQueue<Tile> list;

    for (int y = y_start; y <= y_end; y++)
    {
        for (int x = x_start; x <= x_end; x++)
        {
            Tile info;
            info.x    = x * TILE_DIMENSION;
            info.y    = y * TILE_DIMENSION;
            info.zoom = zoomLevel;

            list.enqueue(info);
        }
    }
    startDownload(list);
}

QPoint OpenstreetmapMapProvider::coordToPixel(const QGeoCoordinate& coord,
    int zoomLevel) const
{
    return QPoint(long2tilex(coord.longitude(), zoomLevel) * TILE_DIMENSION,
               lat2tiley(coord.latitude(), zoomLevel) * TILE_DIMENSION);
}

QGeoCoordinate OpenstreetmapMapProvider::pixelToCoord(const QPoint& point,
    int zoomLevel) const
{
    return QGeoCoordinate(tiley2lat(point.y() / (double)TILE_DIMENSION,
               zoomLevel),
               tilex2long(point.x() / (double)TILE_DIMENSION, zoomLevel));
}

QGeoCoordinate OpenstreetmapMapProvider::moveCoord(const QGeoCoordinate& coord,
    int dx, int dy, int zoomLevel) const
{
    QPoint p = coordToPixel(coord, zoomLevel);

    p.setX(p.x() + dx);
    p.setY(p.y() + dy);
    return pixelToCoord(p, zoomLevel);
}

int OpenstreetmapMapProvider::minZoomLevel() const
{
    return 0;
}

int OpenstreetmapMapProvider::maxZoomLevel() const
{
    return 18;
}

void OpenstreetmapMapProvider::startDownload(QQueue<Tile>& queue)
{
    mIsRunning = true;

    while (queue.size() > 0)
    {
        Tile info = queue.dequeue();
        fetchTile(info);
    }
}

void OpenstreetmapMapProvider::cancelDownload()
{
    // TODO protect by mutex
    mIsRunning = false;
}

void OpenstreetmapMapProvider::fetchTile(Tile info)
{
    int     x = info.x / TILE_DIMENSION;
    int     y = info.y / TILE_DIMENSION;

    QString url = QString("%1/%2/%3/%4.png").arg(TILESERVER_ENDPOINT1)
        .arg(info.zoom).arg(x).arg(y);
    QString key = QString("%2/%3/%4.png").arg(info.zoom).arg(x).arg(y);

    // check local cache first.

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

    emit tileAvailable(wrapper->info);

    delete wrapper;
    reply->deleteLater();
}

double OpenstreetmapMapProvider::long2tilex(double lon, int z) const
{
    return (lon + 180.0) / 360.0 * pow(2.0, z);
}

double OpenstreetmapMapProvider::lat2tiley(double lat, int z) const
{
    return (1.0 - log( tan(lat * M_PI / 180.0) + 1.0 /
           cos(lat * M_PI / 180.0)) / M_PI) / 2.0 * pow(2.0, z);
}

double OpenstreetmapMapProvider::tilex2long(double x, int z) const
{
    return x / pow(2.0, z) * 360.0 - 180;
}

double OpenstreetmapMapProvider::tiley2lat(double y, int z) const
{
    double n = M_PI - 2.0 * M_PI * y / pow(2.0, z);

    return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}
}
