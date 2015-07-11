#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QObject>
#include <QWidget>
#include <QGeoCoordinate>
#include <QList>

#include "mapprovider.h"

namespace MapView
{
class MapView : public QWidget
{
    Q_OBJECT

    public:

        explicit MapView(QWidget* parent = 0);
        virtual ~MapView();

        QSize sizeHint() const;
        void setCurrentCoord(const QGeoCoordinate& coord);

    signals:

    public slots:

        void setMapProvider(MapProvider* provider);

    protected:

        void paintEvent(QPaintEvent*);
        void resizeEvent(QResizeEvent*);

    private slots:

        void onTileAvailable(const TileInfo& info);

    private:

        MapProvider*       mMapProvider;
        QList<TileInfo> mTileInfoList;
        QGeoCoordinate     mCurrentCoord;
        QImage             mIconMapPin;
        int                mZoomLevel;
        QRect              mTileBounds;

        void computeTileBounds();
};
}

#endif // MAPVIEW_H
