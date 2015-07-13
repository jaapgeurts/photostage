#ifndef MAPVIEW_IMAGEMARKER_H
#define MAPVIEW_IMAGEMARKER_H

#include <QObject>
#include <QImage>

#include "abstractmarker.h"

namespace MapView
{
class ImageMarker : public AbstractMarker
{
    public:

        ImageMarker(QObject* parent = 0);
        ImageMarker(const QImage& img,
            const QGeoCoordinate& coord,
            QObject* parent);

        QRect bounds() const;
        void paint(QPainter* painter);

    private:

        QImage mIcon;
};
}
#endif // IMAGEMARKER_H
