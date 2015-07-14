#include "photomarker.h"

namespace PhotoStage
{
PhotoMarker::PhotoMarker(QObject* parent) :
    AbstractMarker(parent)
{
    setAnchor(AbstractMarker::BottomLeft);
    pp.addRoundedRect(0, 0, 24, 24, 8, 8);
    pp.moveTo(0, 20);
    pp.lineTo(0, 32);
    pp.lineTo(8, 20);
    pp.lineTo(0, 20);
    pp.setFillRule(Qt::WindingFill);
}

PhotoMarker::PhotoMarker(const QGeoCoordinate& coord,
    QObject* parent) :
    AbstractMarker(coord, parent)
{
}

QSize PhotoMarker::size() const
{
    return QSize(24, 24 + 8);
}

void PhotoMarker::paint(QPainter* painter)
{
    //    painter->setBrush(QBrush(Qt::red));
    painter->setPen(Qt::black);
    painter->fillPath(pp, QBrush(Qt::red));
}
}
