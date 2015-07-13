#ifndef MAPVIEW_ABSTRACTMARKER_H
#define MAPVIEW_ABSTRACTMARKER_H

#include <QPainter>
#include <QObject>
#include <QGeoCoordinate>

#include "QRect"

namespace MapView
{
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
        AbstractMarker(const QGeoCoordinate& coord, QObject* parent = 0);
        virtual ~AbstractMarker();

        void setCoord(const QGeoCoordinate& coord);
        const QGeoCoordinate& coord() const;

        virtual QRect bounds() const = 0;
        Anchor anchor() const;
        void setAnchor(Anchor anchor);

        virtual void paint(QPainter* painter) = 0;

    private:

        Anchor         mAnchor;
        QGeoCoordinate mCoord;
};
}
#endif // ABSTRACTMARKER_H
