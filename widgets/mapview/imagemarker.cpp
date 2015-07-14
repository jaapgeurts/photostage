#include "imagemarker.h"

namespace MapView
{
ImageMarker::ImageMarker(QObject* parent) :
    AbstractMarker(parent)
{
}

ImageMarker::ImageMarker(const QImage& img,
    const QGeoCoordinate& coord,
    QObject* parent) :
    AbstractMarker(coord, parent),
    mIcon(img)
{
}

QSize ImageMarker::size() const
{
    return mIcon.size();
}

void ImageMarker::paint(QPainter* painter)
{
    //painter->drawText(10,10,"X");
    painter->drawImage(0, 0, mIcon);
}
}
