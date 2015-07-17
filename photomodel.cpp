#include <QImage>
#include <QDir>
#include <QDebug>

#include "constants.h"
#include "photomodel.h"
#include "widgets/tileview.h"
#include "widgets/mapview/layer.h"

namespace PhotoStage
{
PhotoModel::PhotoModel(QObject* parent) :
    QAbstractListModel(parent)
{
    mWorkUnit = PhotoWorkUnit::instance();

    mPreviewCache = PreviewCache::globalCache();

    mLoader = ImageFileLoader::getLoader();
    connect(mLoader, &ImageFileLoader::dataReady, this,
        &PhotoModel::imageLoaded);
    // connect(mLoader, &ImageFileLoader::error, this, &PhotoModel::onImageFailed);
}

PhotoModel::~PhotoModel()
{
    mPhotoList.clear();
}

QVariant PhotoModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() > mPhotoList.size())
    {
        qDebug() << "SqlPhotoModel::data() Invalid row requested";
        return QVariant();
    }

    Photo photo;

    switch (role)
    {
        case Qt::DisplayRole:
            return QString(mPhotoList.at(index.row()).srcImagePath());

        case TileView::TileView::PhotoRole:
        case Photo::DataRole:
        case MapView::Layer::DataRole:
            photo = mPhotoList.at(index.row());
            return QVariant::fromValue<Photo>(photo);

        case MapView::Layer::GeoCoordinateRole:
            return QVariant::fromValue<QGeoCoordinate>(mPhotoList.at(index.
                       row()).exifInfo().location);

        default:
            return QVariant();
    }
}

void PhotoModel::loadImage(Photo& photo)
{
    QString key = QString::number(photo.id());
    QImage  img = mPreviewCache->get(key);

    if (!img.isNull())
    {
        photo.setLibraryPreview(img);
        photo.setOriginal(img);
    }
    else if (!photo.isDownloading())
    {
        // load image in background thread.
        // add to thread queue so that only 1 instance of Halide runs
        photo.setIsDownloading(true);
        mLoader->addJob(QVariant::fromValue<Photo>(photo),
            photo.srcImagePath());
    }
}

void PhotoModel::imageLoaded(const QVariant& ref, const QImage& image)
{
    if (image.isNull())
        return;

    Photo photo = ref.value<Photo>();

    // get actual width x height & store in db.
    photo.exifInfo().width  = image.width();
    photo.exifInfo().height = image.height();
    PhotoWorkUnit::instance()->updateExifInfo(photo);

    QImage preview = image.scaled(QSize(PREVIEW_IMG_WIDTH, PREVIEW_IMG_HEIGHT),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QString key = QString::number(photo.id());

    mPreviewCache->put(key, preview);

    // TODO: original = null
    photo.setLibraryPreview(preview);
    photo.setIsDownloading(false);

    // find the index for this photo.
    QVector<int> roles;
    emit         dataChanged(mPhotoIndexMap.value(photo.id()),
        mPhotoIndexMap.value(photo.id()), roles);
}

void PhotoModel::onReloadPhotos(PhotoModel::SourceType source,
    long long pathId)
{
    beginResetModel();

    if (source == SourceFiles)
    {
        mPhotoList = mWorkUnit->getPhotosByPath(pathId);
        mPhotoIndexMap.clear();
        int i = 0;
        foreach(Photo p, mPhotoList)
        {
            p.setOwner(this);
            mPhotoIndexMap.insert(p.id(), index(i));
            i++;
        }
    }
    else if (source == SourceCollection)
    {
        // TODO: not yet implemented
        //mPhotoInfoList = mWorkUnit->getPhotosByCollection(id);
    }
    // should also cancel all open threads
    endResetModel();
}

bool PComp(const Photo& a, const Photo& b)
{
    return a.id() < b.id();
}

void PhotoModel::refreshData(const QList<Photo>& list )
{
    // for now just emit that all data has changed. the tileview doesnt check anyway
    emit dataChanged(index(0, 0), index(rowCount(QModelIndex()) - 1, 0));
}

void PhotoModel::addData(const QList<long long>& idList)
{
    // figure out what was changed, what is new and what has been added
    int          start = rowCount(QModelIndex());

    QList<Photo> list = mWorkUnit->getPhotosById(idList);
    beginInsertRows(QModelIndex(), start, list.size() - 1);
    Photo        info;
    foreach(info, list)
    {
        mPhotoList.append(info);
    }
    //    insertRows(start,list.size());
    endInsertRows();
}

QVariant PhotoModel::headerData(int /*section*/,
    Qt::Orientation /*orientation*/,
    int /*role*/) const
{
    return QVariant();
}

int PhotoModel::rowCount(const QModelIndex& /*parent*/) const
{
    return mPhotoList.size();
}
}
