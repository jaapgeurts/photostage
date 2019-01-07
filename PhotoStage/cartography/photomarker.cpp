#include "photomarker.h"

namespace PhotoStage
{
PhotoMarker::PhotoMarker(QObject* parent) : AbstractMarker(parent)
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

void PhotoMarker::paint(QPainter& painter, const MapView::MarkerInfo& info, const QVariant& /*data*/)
{
    painter.setRenderHint(QPainter::Antialiasing);
    QBrush b;

    if (info.markerState == MapView::MarkerInfo::MarkerStateSelected)
        b = QBrush(QColor(250, 170, 0));
    else if (info.markerState == MapView::MarkerInfo::MarkerStateNone)
        b = QBrush(Qt::red);
    painter.setBrush(b);
    painter.setPen(Qt::black);
    painter.fillPath(pp, b);
    painter.drawPath(pp);
    painter.drawText(8, 16, "1");
}
}
