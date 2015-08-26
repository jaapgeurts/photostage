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
        ImageMarker(const QImage& img, QObject* parent);

        QSize size() const;
        void paint(QPainter& painter, const MarkerInfo&, const QVariant&);

    private:

        QImage mIcon;
};
}
#endif // IMAGEMARKER_H
