#ifndef PHOTOMARKER_H
#define PHOTOMARKER_H

#include <QPainterPath>

#include "widgets/mapview/abstractmarker.h"

namespace PhotoStage
{
class PhotoMarker : public MapView::AbstractMarker
{
    public:

        PhotoMarker(QObject* parent = 0);
        PhotoMarker(const QGeoCoordinate& coord, QObject* parent = 0);

        QSize size() const;
        void paint(QPainter* painter);

        void setSelected(bool selected);

    private:

        QPainterPath pp;
        bool         mSelected;
};
}
#endif // PHOTOMARKER_H
