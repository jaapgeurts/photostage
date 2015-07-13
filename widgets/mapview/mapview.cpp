#include <QPainter>
#include <QDebug>

#include "layer.h"
#include "mapview.h"
#include "openstreetmapmapprovider.h"

namespace MapView
{
MapView::MapView(QWidget* parent) :
    QWidget(parent),
    mZoomLevel(8)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    mZoomSlider = new QSlider(Qt::Vertical, this);
    mZoomSlider->resize(30, 150);
    connect(mZoomSlider, &QSlider::valueChanged,
        this, &MapView::onZoomLevelChanged);
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
    mZoomSlider->setMinimum(mMapProvider->minZoomLevel());
    mZoomSlider->setMaximum(mMapProvider->maxZoomLevel());
    mZoomSlider->setValue(mZoomLevel);
}

int MapView::zoomLevel() const
{
    return mZoomLevel;
}

QPoint MapView::origin() const
{
    return mOrigin;
}

void MapView::setZoomLevel(int level)
{
    mZoomLevel = level;
}

MapProvider* MapView::mapProvider() const
{
    return mMapProvider;
}

void MapView::onTileAvailable(const Tile& info)
{
    qDebug () << "New tiles";
    mTileList.append(info);
    computeTileBounds();
    update();
}

void MapView::computeTileBounds()
{
    int minx, miny, maxx, maxy;

    minx = miny = std::numeric_limits<int>::max();
    maxx = maxy = std::numeric_limits<int>::min();

    foreach(Tile info, mTileList)
    {
        if (info.x < minx)
            minx = info.x;

        if (info.x > maxx)
            maxx = info.x;

        if (info.y < miny)
            miny = info.y;

        if (info.y > maxy)
            maxy = info.y;

        mTileBounds.setTop(minx);
        mTileBounds.setLeft(miny);
        mTileBounds.setRight(maxx);
        mTileBounds.setBottom(maxy);
    }
}

void MapView::onZoomLevelChanged(int value)
{
    // currentCoord should be the pixel under the mouse or the center of the screen.
    mZoomLevel    = value;
    mCurrentCoord = mMapProvider->moveCoord(mCurrentCoord,
            -width() / 2, -height() / 2, mZoomLevel);
    mOrigin = mMapProvider->coordToPixel(mCurrentCoord, mZoomLevel);

    fetchTiles();
}

void MapView::setCurrentCoord(const QGeoCoordinate& coord)
{
    if (coord.isValid())
    {
        mCurrentCoord = mMapProvider->moveCoord(coord,
                -width() / 2, -height() / 2, mZoomLevel);
        mOrigin = mMapProvider->coordToPixel(mCurrentCoord, mZoomLevel);

        fetchTiles();
    }
    else
        qDebug() << "Invalid Geo Coord";
}

void MapView::fetchTiles()
{
    if (mCurrentCoord.isValid())
    {
        mTileList.clear();
        mMapProvider->getTiles(mCurrentCoord, mZoomLevel, width(), height());
    }
}

void MapView::addLayer(Layer* layer)
{
    mLayers.append(layer);
}

void MapView::resizeEvent(QResizeEvent*)
{
    if (mCurrentCoord.isValid())
    {
        if (!mTileBounds.contains(rect()))
        {
            mTileList.clear();

            mMapProvider->getTiles(mCurrentCoord, mZoomLevel, width(),
                height());
        }
    }
    mZoomSlider->resize(30, height() / 2 - 60);
    mZoomSlider->move(width() - 40, 30);
    update();
}

void MapView::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    int      w = width();
    int      h = height();

    // First draw the bottom fixed layer (map tiles)
    foreach(const Tile &info, mTileList)
    {
        QImage img = info.image;

        if (!img.isNull())
        {
            painter.drawImage(info.x - mOrigin.x(), info.y - mOrigin.y(), img);
        }
    }

    // draw other layers
    foreach(Layer * layer, mLayers)
    {
        layer->paint(&painter);
    }
}
}
