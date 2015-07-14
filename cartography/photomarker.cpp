#include "photomarker.h"

namespace PhotoStage
{
PhotoMarker::PhotoMarker(QObject* parent) :
    AbstractMarker(parent)
{
    setAnchor(AbstractMarker::BottomLeft);
    pp.moveTo(0, 6);
    pp.arcTo(0, 0, 12, 12, 180, -90);
    pp.lineTo(18, 0);
    pp.arcTo(12, 0, 12, 12, 90, -90);
    pp.lineTo(24, 18);
    pp.arcTo(12, 12, 12, 12, 0, -90);
    pp.lineTo(8, 24);
    pp.lineTo(0, 32);
    pp.lineTo(0, 6);
}

PhotoMarker::PhotoMarker(const QGeoCoordinate& coord,
    QObject* parent) :
    AbstractMarker(coord, parent)
{
    setAnchor(AbstractMarker::BottomLeft);
    pp.moveTo(0, 6);
    pp.arcTo(0, 0, 12, 12, 180, -90);
    pp.lineTo(18, 0);
    pp.arcTo(12, 0, 12, 12, 90, -90);
    pp.lineTo(24, 18);
    pp.arcTo(12, 12, 12, 12, 0, -90);
    pp.lineTo(8, 24);
    pp.lineTo(0, 32);
    pp.lineTo(0, 6);
}

QSize PhotoMarker::size() const
{
    return QSize(24, 24 + 8);
}

void PhotoMarker::paint(QPainter* painter)
{
    painter->setBrush(QBrush(Qt::red));
    painter->setPen(Qt::black);
    painter->fillPath(pp, QBrush(Qt::red));
    painter->drawPath(pp);
    painter->drawText(8,16,"1");
}
}
