#include <QPainter>
#include <QDebug>

#include "mapview.h"
#include "openstreetmapmapprovider.h"

namespace PhotoStage
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

void MapView::onTileAvailable(const MapTileInfo& info)
{
    mTileInfoList.append(info);
    qDebug() << "Tile (" << info.canvas_x << "," << info.canvas_y << ")";
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
        mTileInfoList.clear();
        mMapProvider->getTiles(mCurrentCoord, mZoomLevel, width(), height());
    }
}

void MapView::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    int      w = width();
    int      h = height();

    foreach(const MapTileInfo &info, mTileInfoList)
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
