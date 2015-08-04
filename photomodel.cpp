#include <QImage>
#include <QDir>
#include <QDebug>
#include <QItemSelectionRange>

#include "constants.h"
#include "photomodel.h"
#include "preferences.h"

#include "widgets/tileview.h"
#include "widgets/mapview/layer.h"

#include "jobs/previewgeneratorjob.h"
#include "jobs/colortransformjob.h"
#include "jobs/previewcacheloaderjob.h"

namespace PhotoStage
{
PhotoModel::PhotoModel(QObject* parent) :
    QAbstractListModel(parent),
    mRootPathId(-1)
{
    mWorkUnit = PhotoDAO::instance();

    mThreadQueue = new ThreadQueue();
}

PhotoModel::~PhotoModel()
{
    mPhotoList.clear();
    delete mThreadQueue;
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

        case Photo::DateTimeRole:
        {
            Nullable<QDateTime> dt = mPhotoList.at(index.row()).exifInfo().dateTimeOriginal;

            if (dt != nullptr)
                return QVariant::fromValue<QDateTime>(*dt);
            else
                return QVariant::fromValue<QDateTime>(QDateTime());
        }

        case Photo::FilenameRole:
            return QString(mPhotoList.at(index.row()).srcImagePath());

        case Widgets::TileView::ImageRole:
        case Photo::DataRole:
        case MapView::Layer::DataRole:
            photo = mPhotoList.at(index.row());
            return QVariant::fromValue<Photo>(photo);

        case MapView::Layer::GeoCoordinateRole:
        {
            Nullable<QGeoCoordinate> coord = mPhotoList.at(index.row()).exifInfo().location;

            if (coord != nullptr)
                return QVariant::fromValue<QGeoCoordinate>(*coord);
            else
                return QVariant::fromValue<QGeoCoordinate>(QGeoCoordinate());
        }

        default:
            return QVariant();
    }
}

void PhotoModel::loadImage(Photo& photo)
{
    if (photo.isDownloading())
        return;

    photo.setIsDownloading(true);

    PreviewCacheLoaderJob* plj = new PreviewCacheLoaderJob(photo);

    plj->connect(plj, &PreviewCacheLoaderJob::imageReady, this, &PhotoModel::previewLoaded);
    uint32_t id = mThreadQueue->addJob(plj);
    mRunningThreads.insert(photo.id(), id);
}

void PhotoModel::convertImage(Photo& photo)
{
    if (photo.isDownloading())
        return;

    photo.setIsDownloading(true);
    ColorTransformJob* cfj = new ColorTransformJob(photo);

    // convert the image to sRGB
    cfj->connect(cfj, &ColorTransformJob::imageReady, this, &PhotoModel::imageTranslated);
    uint32_t id = mThreadQueue->addJob(cfj);
    mRunningThreads.insert(photo.id(), id);
}

void PhotoModel::previewLoaded(Photo photo, const QImage& image)
{
    if (!image.isNull())
    {
        // The image was succesfully loaded from disk cache
        photo.setLibraryPreview(image);
        photo.setOriginal(image);

        ColorTransformJob* cfj = new ColorTransformJob(photo);

        // convert the image to sRGB
        cfj->connect(cfj, &ColorTransformJob::imageReady, this, &PhotoModel::imageTranslated);
        uint32_t id = mThreadQueue->addJob(cfj);

        mRunningThreads.insert(photo.id(), id);
    }
    else
    {
        // no image available in the cache.
        // generate a preview from the original image
        PreviewGeneratorJob* il = new PreviewGeneratorJob(photo);
        il->connect(il, &PreviewGeneratorJob::imageReady, this, &PhotoModel::previewGenerated);
        uint32_t             id = mThreadQueue->addJob(il);
        mRunningThreads.insert(photo.id(), id);
    }
}

void PhotoModel::previewGenerated(Photo photo, const QImage& image)
{
    if (image.isNull())
    {
        photo.setIsDownloading(false);
        mRunningThreads.remove(photo.id());
        return;
    }

    // get actual width x height & store in db.
    photo.exifInfo().width  = image.width();
    photo.exifInfo().height = image.height();
    PhotoDAO::instance()->updateExifInfo(photo);

    QImage scaled;

    if (image.width() > PREVIEW_IMG_WIDTH &&
        image.height() > PREVIEW_IMG_HEIGHT)
    { // only scale images down. never scale up.
        scaled = image.scaled(PREVIEW_IMG_WIDTH, PREVIEW_IMG_HEIGHT,
                Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        scaled = image;
    }
    QString key = QString::number(photo.id());

    PreviewCache::globalCache()->put(key, scaled);

    // TODO: original = null
    photo.setLibraryPreview(scaled);

    ColorTransformJob* cfj = new ColorTransformJob(photo);

    // convert the image to sRGB
    cfj->connect(cfj, &ColorTransformJob::imageReady, this, &PhotoModel::imageTranslated);
    uint32_t id = mThreadQueue->addJob(cfj);
    mRunningThreads.insert(photo.id(), id);
}

void PhotoModel::imageTranslated(Photo photo, const QImage& image)
{
    if (!image.isNull())
    {
        photo.setLibraryPreviewsRGB(image);

        // find the index for this photo.
        QVector<int> roles;
        emit         dataChanged(mPhotoIndexMap.value(photo.id()), mPhotoIndexMap.value(photo.id()), roles);
    }
    mRunningThreads.remove(photo.id());
    photo.setIsDownloading(false);
}

void PhotoModel::setRootPath(PhotoModel::SourceType source, long long pathId)
{
    beginResetModel();
    mRootPathId = pathId;

    // cancel all running jobs
    mThreadQueue->cancel();

    if (source == SourceFiles)
    {
        mPhotoList = mWorkUnit->getPhotosByPath(pathId, Preferences::instance()->library_include_subfolders);
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

long long PhotoModel::rootPath()
{
    return mRootPathId;
}

Qt::DropActions PhotoModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

void PhotoModel::onVisibleTilesChanged(const QModelIndex& start, const QModelIndex& end)
{
    QList<uint32_t> idList;

    // bail if range is invalid
    if (start.row() <= 0 || end.row() <= 0)
        return;

    // cancel jobs that are not visible
    // by making a list of jobs that are currently in view.
    for (int i = start.row(); i <= end.row(); i++)
    {
        Photo p = mPhotoList.at(i);
        // TODO: race condition here. the value could be gone
        // between the check and reading it.
        uint32_t id = mRunningThreads.value(p.id());

        if (id > 0)     // if it is 0 it has already completed. 0 is not a valid thread id
            idList.append(id);
    }

    if (!idList.isEmpty())
    {
        // then purge all jobs but keep the id's in the list.
        mThreadQueue->purgeExcept(idList);
    }
}

bool PComp(const Photo& a, const Photo& b)
{
    return a.id() < b.id();
}

void PhotoModel::refreshData(const QList<Photo>& list)
{
    // for now just emit that all data has changed. the tileview doesnt check anyway
    emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
}

void PhotoModel::addData(const QList<long long>& idList)
{
    // figure out what was changed, what is new and what has been added
    int start = rowCount();
    int count = idList.size();

    qDebug() << "Imported list dump" << idList;

    QList<Photo> list = mWorkUnit->getPhotosById(idList);
    beginInsertRows(QModelIndex(), start, start + count - 1);
    foreach(Photo info, list)
    {
        info.setOwner(this);
        mPhotoList.append(info);
    }
    endInsertRows();
}

bool PhotoModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (row >= mPhotoList.size() || row + count  > mPhotoList.size() || row < 0 || count < 0)
        return false;

    emit beginRemoveRows(parent, row, row + count - 1);

    for (int i = row; i < row + count; i++)
        mPhotoList.removeAt(i);
    emit endRemoveRows();

    return true;
}

QList<Photo> PhotoModel::toList() const
{
    return mPhotoList;
}

QList<Photo> PhotoModel::toList(const QItemSelection& selection) const
{
    QList<Photo> list;

    foreach(QModelIndex index, selection.indexes())
    {
        list.append(mPhotoList.at(index.row()));
    }
    return list;
}

QVariant PhotoModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int /*role*/) const
{
    return QVariant();
}

int PhotoModel::rowCount(const QModelIndex& /*parent*/) const
{
    return mPhotoList.size();
}
}
