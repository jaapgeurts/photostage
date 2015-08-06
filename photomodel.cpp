#include <QImage>
#include <QDir>
#include <QDebug>
#include <QItemSelectionRange>

#include "constants.h"
#include "dragdropinfo.h"
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
    mRootId(-1),
    mThreadQueue(new ThreadQueue())
{
    mWorkUnit = DatabaseAccess::photoDao();
    DatabaseAccess* dbAccess = DatabaseAccess::instance();

    connect(dbAccess, &DatabaseAccess::photosAdded, this, &PhotoModel::onPhotosAdded);
    connect(dbAccess, &DatabaseAccess::photosDeleted, this, &PhotoModel::onPhotosDeleted);
    connect(dbAccess, &DatabaseAccess::photosChanged, this, &PhotoModel::onPhotosChanged);
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

    plj->connect(plj, &PreviewCacheLoaderJob::imageReady, this, &PhotoModel::onPreviewLoaded);
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
    cfj->connect(cfj, &ColorTransformJob::imageReady, this, &PhotoModel::onImageTranslated);
    uint32_t id = mThreadQueue->addJob(cfj);
    mRunningThreads.insert(photo.id(), id);
}

void PhotoModel::onPreviewLoaded(Photo photo, const QImage& image)
{
    if (!image.isNull())
    {
        // The image was succesfully loaded from disk cache
        photo.setLibraryPreview(image);
        photo.setOriginal(image);

        ColorTransformJob* cfj = new ColorTransformJob(photo);

        // convert the image to sRGB
        cfj->connect(cfj, &ColorTransformJob::imageReady, this, &PhotoModel::onImageTranslated);
        uint32_t id = mThreadQueue->addJob(cfj);

        mRunningThreads.insert(photo.id(), id);
    }
    else
    {
        // no image available in the cache.
        // generate a preview from the original image
        PreviewGeneratorJob* il = new PreviewGeneratorJob(photo);
        il->connect(il, &PreviewGeneratorJob::imageReady, this, &PhotoModel::onPreviewGenerated);
        uint32_t             id = mThreadQueue->addJob(il);
        mRunningThreads.insert(photo.id(), id);
    }
}

void PhotoModel::onPreviewGenerated(Photo photo, const QImage& image)
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
    DatabaseAccess::photoDao()->updateExifInfo(photo);

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
    cfj->connect(cfj, &ColorTransformJob::imageReady, this, &PhotoModel::onImageTranslated);
    uint32_t id = mThreadQueue->addJob(cfj);
    mRunningThreads.insert(photo.id(), id);
}

void PhotoModel::onImageTranslated(Photo photo, const QImage& image)
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

void PhotoModel::setRootPath(PhotoModel::SourceType source, long long id)
{
    beginResetModel();

    mRootId      = id;
    mPhotoSource = source;

    // cancel all running jobs
    mThreadQueue->cancel();

    if (source == SourceFiles)
    {
        mPhotoList = mWorkUnit->getPhotosByPath(id, Preferences::instance()->library_include_subfolders);
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
        mPhotoList = mWorkUnit->getPhotosByCollectionId(id, Preferences::instance()->library_include_subfolders);
        mPhotoIndexMap.clear();
        int i = 0;
        foreach(Photo p, mPhotoList)
        {
            p.setOwner(this);
            mPhotoIndexMap.insert(p.id(), index(i));
            i++;
        }
    }
    // should also cancel all open threads
    endResetModel();
}

long long PhotoModel::rootPath()
{
    return mRootId;
}

Qt::DropActions PhotoModel::supportedDragActions() const
{
    return Qt::CopyAction;
}

Qt::DropActions PhotoModel::supportedDropActions() const
{
    return Qt::DropActions();
}

QStringList PhotoModel::mimeTypes() const
{
    QStringList list;

    list << MIMETYPE_TILEVIEW_SELECTION;
    return list;
}

QMimeData* PhotoModel::mimeData(const QModelIndexList& indexes) const
{
    QMimeData* mimeData = new QMimeData();

    // tile from which the drag started
    //    Photo dragPhoto = data(index, Widgets::TileView::ImageRole).value<Photo>();

    //    image = QPixmap::fromImage(dragPhoto.libraryPreview().
    //            scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //    hotspot = QPoint(image.width() / 2, image.height() / 2);

    // get the id's of all selected items
    QList<long long> idlist;
    foreach(QModelIndex index, indexes)
    {
        Photo p = data(index, Widgets::TileView::ImageRole).value<Photo>();

        idlist << p.id();
    }

    DragDropInfo info(DragDropInfo::PhotoModel, idlist);
    mimeData->setData(MIMETYPE_TILEVIEW_SELECTION, info.toByteArray());
    return mimeData;
}

bool PhotoModel::canDropMimeData(const QMimeData* data,
    Qt::DropAction action, int row, int column, const QModelIndex& parent) const
{
    // do not support drops for now.
    return false;
}

bool PhotoModel::dropMimeData(const QMimeData* data,
    Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
    return false;
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

void PhotoModel::onPhotosAdded(long long pathid, long long photoid)
{
    if (mPhotoSource == SourceFiles && pathid == mRootId)
    {
        int              start = rowCount();

        QList<long long> idList;
        idList << photoid;
        QList<Photo>     list = mWorkUnit->getPhotosById(idList);

        beginInsertRows(QModelIndex(), start, start );
        foreach(Photo info, list)
        {
            info.setOwner(this);
            mPhotoList.append(info);
        }
        endInsertRows();
    }
}

void PhotoModel::onPhotosDeleted(const QList<Photo>& photos)
{
    foreach(Photo photo, photos)
    {
        int count = mPhotoList.size();
        int i;

        for (i = 0; i < count; i++)
            if (photo.id() == mPhotoList.at(i).id())
                break;


        beginRemoveRows(QModelIndex(), i, i);
        mPhotoList.removeAt(i);
        endRemoveRows();
    }
}

void PhotoModel::onPhotosChanged(const QList<Photo>& photos)
{
    foreach(Photo photo, photos)
    {
        int count = mPhotoList.size();
        int i;

        for (i = 0; i < count; i++)
            if (photo.id() == mPhotoList.at(i).id())
                emit dataChanged(index(i), index(i));

    }
}

bool PComp(const Photo& a, const Photo& b)
{
    return a.id() < b.id();
}

void PhotoModel::refreshData(const QList<Photo>& list)
{
    // for now just emit that all data has changed. the tileview doesnt check anyway
    emit dataChanged(index(0), index(rowCount() - 1));
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
