#ifndef MAP_H
#define MAP_H

#include "module.h"
#include "photo.h"

#include "widgets/mapprovider.h"

namespace Ui
{
class Map;
}

namespace PhotoStage
{
class Map : public Module
{
    Q_OBJECT

    public:

        explicit Map(QWidget* parent = 0);
        ~Map();

        QRect lightGap();

        void setPhotos(const QList<Photo>& photos);

    private:

        Ui::Map*     ui;

        QList<Photo> mPhotos;
        MapView::MapProvider* mMapProvider;
};
}
#endif // MAP_H
