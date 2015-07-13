#include "map.h"
#include "ui_map.h"

#include "widgets/mapview/openstreetmapmapprovider.h"
#include "widgets/tileview.h"

namespace PhotoStage
{
Map::Map(QAbstractItemModel* model, QWidget* parent) :
    Module(parent),
    ui(new Ui::Map),
    mPhotoModel(model),
    mLoadPhoto(false)
{
    ui->setupUi(this);
    mMapProvider = new MapView::OpenstreetmapMapProvider(ui->mapView);
    mLayer       = new MapView::Layer(mMapProvider);
    ui->mapView->setMapProvider(mMapProvider);

    connect(mPhotoModel, &QAbstractItemModel::modelReset,
        this, &Map::onModelReset);
}

Map::~Map()
{
    delete ui;
}

QRect Map::lightGap()
{
    // TODO: implement this
    return QRect();
}

void Map::setSelectionModel(QItemSelectionModel* selectionModel)
{
    connect(selectionModel, &QItemSelectionModel::currentChanged,
        this, &Map::onCurrentPhotoChanged);
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
        this, &Map::onSelectionChanged);
}

void Map::onCurrentPhotoChanged(const QModelIndex& current,
    const QModelIndex& /*previous*/)
{
    mPhoto = mPhotoModel->data(current, TileView::TileView::PhotoRole)
        .value<Photo>();

    // if visible, load immediately
    if (isVisible())
        setPhoto(mPhoto);
    else
        // TODO: if invisible, defer loading until visible
        mLoadPhoto = true;
}

void Map::onSelectionChanged(const QItemSelection& selected,
    const QItemSelection& deselected)
{
}

void Map::onModelReset()
{
    return;
    mLayer->clear();
    int c = mPhotoModel->rowCount();

    for (int i = 0; i < c; i++)
    {
        Photo p = mPhotoModel->data(mPhotoModel->index(c, 0),
                TileView::TileView::PhotoRole).value<Photo>();
        //  if (photo.exifInfo().location.isValid()))
    }
}

void Map::showEvent(QShowEvent*)
{
    if (mLoadPhoto)
    {
        setPhoto(mPhoto);
        mLoadPhoto = false;
    }
}

void Map::setPhoto(Photo photo)
{
    if (photo.exifInfo().location.isValid())
        ui->mapView->setCurrentCoord(photo.exifInfo().location);
}
}
