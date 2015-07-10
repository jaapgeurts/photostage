#include "map.h"
#include "ui_map.h"

#include "widgets/openstreetmapmapprovider.h"

namespace PhotoStage
{
Map::Map(QWidget* parent) :
    Module(parent),
    mMapProvider(new OpenstreetmapMapProvider(this)),
    ui(new Ui::Map)
{
    ui->setupUi(this);
    ui->mapView->setMapProvider(mMapProvider);
}

Map::~Map()
{
    delete ui;
}

QRect Map::lightGap()
{
    // TODO: implement this
}

void Map::setPhotos(const QList<Photo>& photos)
{
    mPhotos = photos;

    if (mPhotos.size() > 0)
    {
        Photo p = mPhotos.at(0);

        if (p.exifInfo().location.isValid())
            ui->mapView->setCurrentCoord(p.exifInfo().location);
    }
}
}
