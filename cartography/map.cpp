#include "map.h"
#include "ui_map.h"

#include "widgets/mapview/openstreetmapmapprovider.h"
#include "widgets/mapview/imagemarker.h"
#include "widgets/tileview.h"

#include "photomarker.h"

namespace PhotoStage
{
Map::Map(QAbstractItemModel* model, QWidget* parent) :
    Module(parent),
    ui(new Ui::Map),
    mPhotoModel(model),
    mLoadPhoto(false),
    mSelectionModel(NULL)
{
    ui->setupUi(this);
    mMapProvider = new MapView::OpenstreetmapMapProvider(ui->mapView);
    mLayer       = new MapView::Layer(ui->mapView);
    mLayer->setDelegate(new PhotoMarker(ui->mapView));
    mLayer->setModel(model);
    ui->mapView->addLayer(mLayer);
    ui->mapView->setMapProvider(mMapProvider);
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

void Map::setModel(QAbstractItemModel* model)
{
    mPhotoModel = model;
    mLayer->setModel(model);
}

void Map::setSelectionModel(QItemSelectionModel* selectionModel)
{
    mSelectionModel = selectionModel;
    connect(selectionModel, &QItemSelectionModel::currentChanged,
        this, &Map::onCurrentPhotoChanged);
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
        this, &Map::onSelectionChanged);
    mLayer->setSelectionModel(selectionModel);
}

void Map::onCurrentPhotoChanged(const QModelIndex& current,
    const QModelIndex& /*previous*/)
{
    if (!current.isValid())
    {
        qDebug() << "No valid current";
        return;
    }

    mPhoto = mPhotoModel->data(current, Widgets::TileView::ImageRole).value<Photo>();

    // if visible, load immediately
    if (isVisible())
        setPhoto(mPhoto);
    else
        // TODO: if invisible, defer loading until visible
        mLoadPhoto = true;
}

void Map::onSelectionChanged(const QItemSelection& selected,
    const QItemSelection& /*deselected*/)
{
    update();
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
    if (photo.exifInfo().location != nullptr)
        ui->mapView->setCurrentCoord(*photo.exifInfo().location);
}
}
