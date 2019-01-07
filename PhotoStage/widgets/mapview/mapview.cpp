#include <QPainter>
#include <QDebug>

#include "layer.h"
#include "mapview.h"
#include "openstreetmapmapprovider.h"

namespace MapView
{
MapView::MapView(QWidget* parent) :
    QWidget(parent),
    mCurrentCoord(),
    mZoomLevel(8),
    mTileBounds(0, 0, 0, 0),
    mIsPanning(false),
    mIsMouseDown(false),
    mDragDelta(0, 0)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    mZoomSlider = new QSlider(Qt::Vertical, this);
    mZoomSlider->resize(30, 150);

    connect(mZoomSlider, &QSlider::valueChanged, this, &MapView::onZoomLevelChanged);

    setContextMenuPolicy(Qt::DefaultContextMenu);

    setCursor(QCursor(Qt::OpenHandCursor));

    // otherwise we don't receive hover events
    setMouseTracking(true);
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
    connect(mMapProvider, &MapProvider::tileAvailable, this, &MapView::onTileAvailable);
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
    grect.setBottomRight(mMapProvider->moveCoord(mOriginCoord, width(), height(), mZoomLevel));
    return grect;
}

void MapView::setZoomLevel(int level)
{
    mZoomLevel = level;
    mZoomSlider->setValue(level);
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
    mOriginCoord  = mMapProvider->moveCoord(mCurrentCoord, -width() / 2, -height() / 2, mZoomLevel);
    mOriginPixels = mMapProvider->coordToPixel(mOriginCoord, mZoomLevel);
    fetchTiles();

    foreach(Layer * layer, mLayers)
    {
        layer->viewChanged();
    }
    emit zoomLevelChanged(value);
}

void MapView::setCurrentCoord(const QGeoCoordinate& coord)
{
    // only reset view location if the coord
    // is not visible on screen already

    if (coord.isValid() && !mapBounds().contains(coord))
    {
        mCurrentCoord = coord;
        mOriginCoord  = mMapProvider->moveCoord(mCurrentCoord, -width() / 2, -height() / 2, mZoomLevel);
        mOriginPixels = mMapProvider->coordToPixel(mOriginCoord, mZoomLevel);
        fetchTiles();

        foreach(Layer * layer, mLayers)
        {
            layer->viewChanged();
        }
    }
}

QGeoCoordinate MapView::currentCoord() const
{
    return mCurrentCoord;
}

void MapView::moveOrigin(const QPoint& delta)
{
    // move the current coord by the current pixels on screen
    mCurrentCoord = mMapProvider->moveCoord(mCurrentCoord,
            delta.x(), delta.y(), mZoomLevel);

    // recompute the origin of the screen
    mOriginCoord  = mMapProvider->moveCoord(mCurrentCoord, -width() / 2, -height() / 2, mZoomLevel);
    mOriginPixels = mMapProvider->coordToPixel(mOriginCoord, mZoomLevel);

    computeTileBounds();

    foreach(Layer * layer, mLayers)
    {
        layer->viewChanged();
    }
    emit centerChanged(mCurrentCoord);
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
    foreach(Layer * layer, mLayers)
    {
        layer->viewChanged();
    }
    update();
}

void MapView::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::RightButton)
        return;

    bool swallowed = false;

    foreach(Layer * layer, mLayers)
    {
        swallowed = layer->mousePressEvent(event);

        if (swallowed)
            break;
    }

    if (!swallowed)
    {
        setCursor(QCursor(Qt::ClosedHandCursor));
        mIsMouseDown        = true;
        mMousePressLocation = event->pos();
    }
}

void MapView::mouseReleaseEvent(QMouseEvent* event)
{
    if (mIsPanning)
    {
        mDragDelta =  mMousePressLocation - event->pos();

        moveOrigin(mDragDelta);
        fetchTiles();
        update();
    }
    setCursor(QCursor(Qt::OpenHandCursor));
    mIsPanning   = false;
    mIsMouseDown = false;
}

void MapView::mouseMoveEvent(QMouseEvent* event)
{
    if (mIsMouseDown)
    {
        QPoint delta = mMousePressLocation - event->pos();

        if (abs(delta.x()) > 5 || abs(delta.y()) > 5)
        {
            mIsPanning   = true;
            mIsMouseDown = false;
        }
    }

    if (mIsPanning)
    {
        mDragDelta =  mMousePressLocation - event->pos();

        moveOrigin(mDragDelta);
        fetchTiles();
        update();

        mMousePressLocation = event->pos();
    }
    else
    {
        // for each layer send the event until eaten.
        foreach(Layer * layer, mLayers)
        {
            if (!layer->mouseMoveEvent(event))
                break;
        }
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

        QGeoCoordinate zoomCoord = mMapProvider->pixelToCoord(pos + mOriginPixels, mZoomLevel);
        mCurrentCoord = mMapProvider->moveCoord(zoomCoord, delta.x() + width() / 2,
                delta.y() + height() / 2, mZoomLevel + steps);

        mZoomSlider->setValue(mZoomLevel + steps);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MapView::contextMenuEvent(QContextMenuEvent* event)
{
    emit contextMenuRequested(mMapProvider->pixelToCoord(event->pos() + mOriginPixels, mZoomLevel), event->pos());
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
        layer->render(painter);
    }
}
}
