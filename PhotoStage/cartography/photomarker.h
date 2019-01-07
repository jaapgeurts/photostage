#ifndef PHOTOSTAGE_PHOTOMARKER_H
#define PHOTOSTAGE_PHOTOMARKER_H

#include <QPainterPath>

#include "widgets/mapview/abstractmarker.h"

namespace PhotoStage
{
class PhotoMarker : public MapView::AbstractMarker
{
    public:

        PhotoMarker(QObject* parent = 0);

        QSize size() const;
        void paint(QPainter& painter, const MapView::MarkerInfo& info, const QVariant&);

    private:

        QPainterPath pp;
};
}
#endif // PHOTOSTAGE_PHOTOMARKER_H
