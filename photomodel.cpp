#include <QImage>
#include <QDir>
#include <QDebug>
#include <QItemSelectionRange>

#include "constants.h"
#include "photomodel.h"

#include "widgets/tileview.h"
#include "widgets/mapview/layer.h"

#include "jobs/previewgeneratorjob.h"
#include "jobs/colortransformjob.h"
#include "jobs/previewcacheloaderjob.h"

namespace PhotoStage
{
PhotoModel::PhotoModel(QObject* parent) :
    QAbstractListModel(parent)
{
    mWorkUnit = PhotoWorkUnit::instance();

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
            return QVariant::fromValue<QDateTime>(
                mPhotoList.at(index.row()).exifInfo().dateTimeOriginal);

        case Photo::FilenameRole:
            return QString(mPhotoList.at(index.row()).srcImagePath());

        case TileView::TileView::ImageRole:
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
    if (photo.isDownloading())
        return;

    photo.setIsDownloading(true);

    PreviewCacheLoaderJob* plj = new PreviewCacheLoaderJob(photo);

    plj->connect(plj, &PreviewCacheLoaderJob::imageReady,
        this, &PhotoModel::previewLoaded);
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
    cfj->connect(cfj, &ColorTransformJob::imageReady,
        this, &PhotoModel::imageTranslated);
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
        cfj->connect(cfj, &ColorTransformJob::imageReady,
            this, &PhotoModel::imageTranslated);
        uint32_t id = mThreadQueue->addJob(cfj);

        mRunningThreads.insert(photo.id(), id);
    }
    else
    {
        // no image available in the cache.
        // generate a preview from the original image
        PreviewGeneratorJob* il = new PreviewGeneratorJob(photo);
        il->connect(il, &PreviewGeneratorJob::imageReady,
            this, &PhotoModel::previewGenerated);
        uint32_t id = mThreadQueue->addJob(il);
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
    PhotoWorkUnit::instance()->updateExifInfo(photo);

    QImage preview = image.scaled(QSize(PREVIEW_IMG_WIDTH, PREVIEW_IMG_HEIGHT),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QString key = QString::number(photo.id());

    PreviewCache::globalCache()->put(key, preview);

    // TODO: original = null
    photo.setLibraryPreview(preview);

    ColorTransformJob* cfj = new ColorTransformJob(photo);

    // convert the image to sRGB
    cfj->connect(cfj, &ColorTransformJob::imageReady,
        this, &PhotoModel::imageTranslated);
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
        emit         dataChanged(mPhotoIndexMap.value(photo.id()),
            mPhotoIndexMap.value(photo.id()), roles);
    }
    mRunningThreads.remove(photo.id());
    photo.setIsDownloading(false);
}

void PhotoModel::onPhotoSourceChanged(PhotoModel::SourceType source,
    long long pathId)
{
    beginResetModel();

    // cancel all running jobs
    mThreadQueue->purgeKeep();

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

void PhotoModel::onVisibleTilesChanged(const QModelIndex& start,
    const QModelIndex& end)
{
    qDebug() << "PhotoModel::onVisibleTilesChanged" << start.row() << "-" <<
        end.row();
    // only cancel jobs that are not visible
    QList<uint32_t> idList;

    if (start.row() > 0 && end.row() > 0)
    {
        for (int i = start.row(); i <= end.row(); i++)
        {
            Photo p = mPhotoList.at(i);
            // TODO: race condition here. the value could be gone
            // between the check and reading it.
            uint32_t id = mRunningThreads.value(p.id());

            if (id > 0) // if it is 0 it has already completed. 0 is not a valid thread id
                idList.append(id);
        }

        if (!idList.isEmpty())
        {
            qDebug() << "Will keep:" << idList;
            qDebug() << "Active running:" << mRunningThreads.values();

            mThreadQueue->purgeKeep(idList);
        }
    }
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
