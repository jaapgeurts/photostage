#include <QPainter>
#include <QDebug>

#include "mapview.h"
#include "openstreetmapmapprovider.h"

namespace MapView
{
MapView::MapView(QWidget* parent) :
    QWidget(parent),
    mIconMapPin(":/icons/map-pin.png"),
    mZoomLevel(14)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

MapView::~MapView()
{
}

QSize MapView::sizeHint() const
{
    return QSize(400, 300);
}

void MapView::setMapProvider(MapProvider* provider)
{
    mMapProvider = provider;
    connect(mMapProvider, &MapProvider::tileAvailable,
        this, &MapView::onTileAvailable);
}

void MapView::computeTileBounds()
{
    int minx, miny, maxx, maxy;

    minx = miny = std::numeric_limits<int>::max();
    maxx = maxy = std::numeric_limits<int>::min();

    foreach(TileInfo info, mTileInfoList)
    {
        if (info.canvas_x < minx)
            minx = info.canvas_x;

        if (info.canvas_x > maxx)
            maxx = info.canvas_x;

        if (info.canvas_y < miny)
            miny = info.canvas_y;

        if (info.canvas_y > maxy)
            maxy = info.canvas_y;

        mTileBounds.setTop(minx);
        mTileBounds.setLeft(miny);
        mTileBounds.setRight(maxx);
        mTileBounds.setBottom(maxy);
    }
}

void MapView::onTileAvailable(const TileInfo& info)
{
    mTileInfoList.append(info);
    //    qDebug() << "Tile (" << info.canvas_x << "," << info.canvas_y << ")";
    computeTileBounds();
    update();
}

void MapView::setCurrentCoord(const QGeoCoordinate& coord)
{
    if (coord.isValid())
    {
        mCurrentCoord = coord;
        mTileInfoList.clear();

        mMapProvider->getTiles(mCurrentCoord, mZoomLevel, width(), height());
    }
    else
        qDebug() << "Invalid Geo Coord";
}

void MapView::resizeEvent(QResizeEvent*)
{
    if (mCurrentCoord.isValid())
    {
        if (!mTileBounds.contains(rect()))
        {
            mTileInfoList.clear();

            mMapProvider->getTiles(mCurrentCoord, mZoomLevel, width(),
                height());
        }
    }
}

void MapView::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    int      w = width();
    int      h = height();

    foreach(const TileInfo &info, mTileInfoList)
    {
        QImage img = info.image;

        if (!img.isNull())
        {
            painter.drawImage(info.canvas_x, info.canvas_y, img);

            painter.drawImage(
                (w - mIconMapPin.width()) / 2,
                h / 2  - mIconMapPin.height(),
                mIconMapPin);
        }
        else
        {
            qDebug() << "Image is NULL";
        }
    }
}
}
