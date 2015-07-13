#ifndef MAPVIEW_ABSTRACTMARKER_H
#define MAPVIEW_ABSTRACTMARKER_H

#include <QPainter>
#include <QObject>

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
        virtual ~AbstractMarker();

        virtual QRect bounds() const = 0;
        Anchor anchor() const;
        void setAnchor(Anchor anchor);

        virtual void paintMarker(QPainter* painter) = 0;

    private:

        Anchor mAnchor;
};
}
#endif // ABSTRACTMARKER_H
