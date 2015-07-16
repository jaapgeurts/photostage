#ifndef MAPVIEW_ABSTRACTMARKER_H
#define MAPVIEW_ABSTRACTMARKER_H

#include <QPainter>
#include <QGeoCoordinate>
#include <QModelIndex>

#include "QRect"

namespace MapView
{
struct MarkerInfo
{
    enum MarkerState
    {
        MarkerStateNone     = 0x000,
        MarkerStateSelected = 0x001,
        MarkerStateActive   = 0x002
    };

    MarkerInfo();

    // the ordinal number in the list
    int index;
    QGeoCoordinate coord; // the position of the marker;
    // the pixel position in the view (relative to the viewport)
    int x;
    int y;
    int width;
    int height;

    QModelIndex modelIndex;

    MarkerState markerState;
};

class AbstractMarker : public QObject
{
    Q_OBJECT

    public:

        enum Anchor
        {
            TopLeft = 1,
            TopRight,
            BottomRight,
            BottomLeft,
            CenterLeft,
            CenterRight,
            CenterTop,
            CenterBottom,
            Center
        };

        AbstractMarker(QObject* parent = 0);
        virtual ~AbstractMarker();

        virtual QSize size() const = 0;
        Anchor anchor() const;
        void setAnchor(Anchor anchor);

        virtual void paint(QPainter& painter,
            const MarkerInfo& info,
            const QVariant& data) = 0;

    private:

        Anchor mAnchor;
};
}
#endif // ABSTRACTMARKER_H
