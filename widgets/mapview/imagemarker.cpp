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

QRect ImageMarker::bounds() const
{
    return QRect(0, 0, mIcon.width(), mIcon.height());
}

void ImageMarker::paint(QPainter* painter)
{
    //painter->drawText(10,10,"X");
    painter->drawImage(0, 0, mIcon);
}
}
