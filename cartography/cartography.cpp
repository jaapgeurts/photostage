#include "cartography.h"
#include "ui_map.h"

#include "widgets/mapview/openstreetmapmapprovider.h"

#include "database/databaseaccess.h"
#include "photomarker.h"
#include "constants.h"

namespace PhotoStage
{
Cartography::Cartography(QAbstractItemModel* model, QWidget* parent) :
    Module(parent),
    ui(new Ui::Map),
    mPhotoModel(model),
    mLoadPhoto(false),
    mSelectionModel(NULL),
    mSavedCoordinate(),
    mZoomLevel(2)
{
    ui->setupUi(this);
    mMapProvider = new MapView::OpenstreetmapMapProvider(ui->mapView);
    mLayer       = new MapView::ModelIndexLayer(ui->mapView);
    mLayer->setDelegate(new PhotoMarker(ui->mapView));
    mLayer->setModel(model);
    ui->mapView->addLayer(mLayer);
    ui->mapView->setMapProvider(mMapProvider);

    connect(ui->mapView, &MapView::MapView::zoomLevelChanged, this, &Cartography::onZoomLevelChanged);
    connect(ui->mapView, &MapView::MapView::centerChanged, this, &Cartography::onCenterChanged);
    connect(ui->mapView, &MapView::MapView::contextMenuRequested, this, &Cartography::onContextMenuRequested);

    mContextMenu = new QMenu(this);

    mContextMenu->addAction("Assign Photos", this, SLOT(onAssignPhotos()));

    // set to last known location, if there is none, set to GMT location.
    QSettings settings;
    settings.beginGroup(SETTINGS_GROUP_CARTOGRAPHY);

    if (settings.contains(SETTINGS_LATITUDE) && settings.contains(SETTINGS_LONGITUDE))
    {
        double latitude  = settings.value(SETTINGS_LATITUDE).toDouble();
        double longitude = settings.value(SETTINGS_LONGITUDE).toDouble();
        mSavedCoordinate = QGeoCoordinate(latitude, longitude);
        mZoomLevel       = settings.value(SETTINGS_ZOOMLEVEL).toInt();
    }

    // if not valid set to GMT location
    if (!mSavedCoordinate.isValid())
    {
        mSavedCoordinate = QGeoCoordinate(0.0, 0.0);
    }
    settings.endGroup();
}

Cartography::~Cartography()
{
    QSettings settings;

    settings.beginGroup(SETTINGS_GROUP_CARTOGRAPHY);

    settings.setValue(SETTINGS_LATITUDE, mSavedCoordinate.latitude());
    settings.setValue(SETTINGS_LONGITUDE, mSavedCoordinate.longitude());
    settings.setValue(SETTINGS_ZOOMLEVEL, mZoomLevel);

    settings.endGroup();

    delete ui;
}

QRect Cartography::lightGap()
{
    // TODO: implement this
    return QRect();
}

void Cartography::setModel(QAbstractItemModel* model)
{
    mPhotoModel = model;
    mLayer->setModel(model);
}

void Cartography::setSelectionModel(QItemSelectionModel* selectionModel)
{
    mSelectionModel = selectionModel;
    connect(selectionModel, &QItemSelectionModel::currentChanged, this, &Cartography::onCurrentPhotoChanged);
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &Cartography::onSelectionChanged);
    mLayer->setSelectionModel(selectionModel);
}

void Cartography::onCurrentPhotoChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
{
    if (!current.isValid())
        return;

    mPhoto = mPhotoModel->data(current, Photo::DataRole).value<Photo>();

    // if visible, load immediately
    if (isVisible())
        setPhoto(mPhoto);
    else
        // TODO: if invisible, defer loading until visible
        mLoadPhoto = true;
}

void Cartography::onSelectionChanged(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/)
{
    update();
}

void Cartography::onZoomLevelChanged(int level)
{
    mZoomLevel = level;
}

void Cartography::onCenterChanged(const QGeoCoordinate& coord)
{
    mSavedCoordinate = coord;
}

void Cartography::onContextMenuRequested(const QGeoCoordinate& coord, const QPoint& pos)
{
    QModelIndex index = mLayer->indexAt(pos);

    if (!index.isValid())
        qDebug() << "No Valid Index";

    mSelectedCoordinate = coord;

    mContextMenu->popup(ui->mapView->mapToGlobal(pos));
}

void Cartography::onAssignPhotos()
{
    QList<Photo> list;
    foreach(QModelIndex index, mSelectionModel->selectedIndexes())
    {
        Photo p = mPhotoModel->data(index, Photo::DataRole).value<Photo>();

        list.append(p);
    }
    DatabaseAccess::photoDao()->assignGeoCoordinate(mSelectedCoordinate, list);
}

void Cartography::showEvent(QShowEvent*)
{
    if (mLoadPhoto)
    {
        setPhoto(mPhoto);
        mLoadPhoto = false;
    }
}

void Cartography::setPhoto(Photo photo)
{
    QGeoCoordinate coord;

    if (photo.exifInfo().location != nullptr)
        coord = *photo.exifInfo().location;

    if (!coord.isValid() && !ui->mapView->currentCoord().isValid())
    {
        coord = mSavedCoordinate;
        ui->mapView->setZoomLevel(mZoomLevel);
    }

    ui->mapView->setCurrentCoord(coord);
}
}
