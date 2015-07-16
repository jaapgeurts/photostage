#include <QPainter>
#include <QDebug>

#include "layer.h"
#include "mapview.h"
#include "openstreetmapmapprovider.h"

namespace MapView
{
MapView::MapView(QWidget* parent) :
    QWidget(parent),
    mZoomLevel(8),
    mTileBounds(0, 0, 0, 0),
    mPanning(false),
    mDragDelta(0, 0)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    mZoomSlider = new QSlider(Qt::Vertical, this);
    mZoomSlider->resize(30, 150);
    connect(mZoomSlider, &QSlider::valueChanged,
        this, &MapView::onZoomLevelChanged);

    setCursor(QCursor(Qt::OpenHandCursor));
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
    return mOriginPixels;
}

QGeoRectangle MapView::mapBounds() const
{
    QGeoRectangle grect;

    grect.setTopLeft(mOriginCoord);
    grect.setBottomRight(mMapProvider->moveCoord(mOriginCoord,
        width(), height(), mZoomLevel));
    return grect;
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
    // qDebug() << "Tile arrived";
    mTileList.append(info);
    computeTileBounds();
    update();
}

void MapView::computeTileBounds()
{
    int minx, miny, maxx, maxy;

    minx = miny = std::numeric_limits<int>::max();
    maxx = maxy = std::numeric_limits<int>::min();
    mTileBounds.setCoords(0, 0, 0, 0);

    if (mTileList.size() > 0)
    {
        foreach(Tile info, mTileList)
        {
            if (info.x < minx)
                minx = info.x;

            if (info.x + info.w > maxx)
                maxx = info.x + info.w;

            if (info.y < miny)
                miny = info.y;

            if (info.y + info.h > maxy)
                maxy = info.y + info.h;
        }

        mTileBounds.setTop(miny);
        mTileBounds.setLeft(minx);
        mTileBounds.setRight(maxx);
        mTileBounds.setBottom(maxy);

        //    mTileBounds.translate(-mOriginPixels);
        //    qDebug() << "Bounds:" << mTileBounds;
        //    qDebug() << "Origin:" << mOriginPixels;
        //  qDebug() << "Rect:" << rect();
        mTileBounds.translate(-mOriginPixels);
    }
    //  qDebug() << "Translated:" << mTileBounds;
}

void MapView::onZoomLevelChanged(int value)
{
    mZoomLevel = value;
    // invalidate the tile list;
    mTileList.clear();
    computeTileBounds();
    // recalculate origin because of new zoomlevel
    mOriginCoord = mMapProvider->moveCoord(mCurrentCoord,
            -width() / 2, -height() / 2, mZoomLevel);
    mOriginPixels =
        mMapProvider->coordToPixel(mOriginCoord, mZoomLevel);
    fetchTiles();
}

void MapView::setCurrentCoord(const QGeoCoordinate& coord)
{
    if (coord.isValid())
    {
        mCurrentCoord = coord;
        mOriginCoord  = mMapProvider->moveCoord(mCurrentCoord,
                -width() / 2, -height() / 2, mZoomLevel);
        mOriginPixels =
            mMapProvider->coordToPixel(mOriginCoord, mZoomLevel);
        fetchTiles();
    }
    else
        qDebug() << "Invalid Geo Coord";
}

void MapView::moveOrigin(const QPoint& delta)
{
    // move the current coord by the current pixels on screen
    mCurrentCoord = mMapProvider->moveCoord(mCurrentCoord,
            delta.x(), delta.y(), mZoomLevel);

    // recompute the origin of the screen
    mOriginCoord = mMapProvider->moveCoord(mCurrentCoord,
            -width() / 2, -height() / 2, mZoomLevel);
    mOriginPixels = mMapProvider->coordToPixel(mOriginCoord, mZoomLevel);

    computeTileBounds();
}

void MapView::fetchTiles()
{
    if (mCurrentCoord.isValid())
    {
        QRect r = rect();

        if (!mTileBounds.contains(r))
        {
            mTileList.clear();
            mMapProvider->getTiles(mOriginCoord, mZoomLevel, width(), height());
        }
    }
}

void MapView::addLayer(Layer* layer)
{
    mLayers.append(layer);
}

void MapView::resizeEvent(QResizeEvent*)
{
    fetchTiles();

    mZoomSlider->resize(30, height() / 2 - 60);
    mZoomSlider->move(width() - 40, 30);
    update();
}

void MapView::mousePressEvent(QMouseEvent* event)
{
    setCursor(QCursor(Qt::ClosedHandCursor));
    mPanning            = true;
    mMousePressLocation = event->pos();
    mDragDelta.setX(0);
    mDragDelta.setY(0);
}

void MapView::mouseReleaseEvent(QMouseEvent* event)
{
    setCursor(QCursor(Qt::OpenHandCursor));
    mPanning   = false;
    mDragDelta =  mMousePressLocation - event->pos();

    moveOrigin(mDragDelta);
    fetchTiles();
    update();
}

void MapView::mouseMoveEvent(QMouseEvent* event)
{
    if (mPanning)
    {
        mDragDelta =  mMousePressLocation - event->pos();

        moveOrigin(mDragDelta);
        fetchTiles();
        update();

        mMousePressLocation = event->pos();
    }
}

void MapView::wheelEvent(QWheelEvent* event)
{
    // TODO: mac trackpads have small angles (two finger move)
    int    vdelta = event->angleDelta().y() / 8;
    QPoint pos    = event->pos();

    int    steps = vdelta / 15;

    if (event->orientation() == Qt::Vertical)
    {
        qDebug() << "Slider step" << steps;
        // TODO: allow zooming in on mouse location;
        QPoint         delta =  -pos;

        QGeoCoordinate zoomCoord = mMapProvider
            ->pixelToCoord(pos + mOriginPixels, mZoomLevel);
        mCurrentCoord = mMapProvider->moveCoord(zoomCoord,
                delta.x() + width() / 2,
                delta.y() + height() / 2, mZoomLevel + steps);

        mZoomSlider->setValue(mZoomLevel + steps);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MapView::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);

    //    int      w = width();
    //    int      h = height();

    // First draw the bottom fixed layer (map tiles)
    foreach(const Tile &info, mTileList)
    {
        QImage img = info.image;

        if (!img.isNull())
        {
            painter.drawImage( info.x - mOriginPixels.x(),
                info.y - mOriginPixels.y(), img);
        }
    }

    // draw other layers
    foreach(Layer * layer, mLayers)
    {
        layer->paint(painter);
    }
}
}
