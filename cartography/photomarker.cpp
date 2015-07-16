#include "photomarker.h"

namespace PhotoStage
{
PhotoMarker::PhotoMarker(QObject* parent) :
    AbstractMarker(parent),
    mSelected(false)
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
    AbstractMarker(coord, parent),
    mSelected(false)
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
    painter->setRenderHint(QPainter::Antialiasing);
    QBrush b;

    if (mSelected)
        b = QBrush(QColor(250, 170, 0));
    else
        b = QBrush(Qt::red);
    painter->setBrush(b);
    painter->setPen(Qt::black);
    painter->fillPath(pp, b);
    painter->drawPath(pp);
    painter->drawText(8, 16, "1");
}

void PhotoMarker::setSelected(bool selected)
{
    mSelected = selected;
}
}
