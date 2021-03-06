#include <QDebug>
#include <QDir>
#include <QImage>
#include <QItemSelectionRange>

#include "constants.h"
#include "dragdropinfo.h"
#include "photomodel.h"
#include "preferences.h"

#include "widgets/mapview/modelindexlayer.h"
#include "widgets/tileview.h"

#include "jobs/colortransformjob.h"
#include "jobs/imageloaderjob.h"

namespace PhotoStage {
PhotoModel::PhotoModel(QObject* parent)
    : QAbstractListModel(parent), mPreviewThreadQueue(new ThreadQueue()),
      mOriginalThreadQueue(new ThreadQueue()), mRootId(-1)
{
  mWorkUnit                = DatabaseAccess::photoDao();
  DatabaseAccess* dbAccess = DatabaseAccess::instance();

  connect(dbAccess, &DatabaseAccess::photosAdded, this,
          &PhotoModel::onPhotosAdded);
  connect(dbAccess, &DatabaseAccess::photosDeleted, this,
          &PhotoModel::onPhotosDeleted);
  connect(dbAccess, &DatabaseAccess::photosChanged, this,
          &PhotoModel::onPhotosChanged);

  connect(dbAccess, &DatabaseAccess::collectionPhotosRemoved, this,
          &PhotoModel::onPhotosDeletedFromCollection);
}

PhotoModel::~PhotoModel()
{
  mPhotoList.clear();
  delete mPreviewThreadQueue;
  delete mOriginalThreadQueue;
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
      Nullable<QDateTime> dt =
          mPhotoList.at(index.row()).exifInfo().dateTimeOriginal;

      if (dt != nullptr)
        return QVariant::fromValue<QDateTime>(*dt);
      else
        return QVariant::fromValue<QDateTime>(QDateTime());
    }

    case Photo::FilenameRole:
      return QString(mPhotoList.at(index.row()).srcImagePath());

    case Photo::DataRole:
    case MapView::ModelIndexLayer::DataRole:
      photo = mPhotoList.at(index.row());
      return QVariant::fromValue<Photo>(photo);

    case MapView::ModelIndexLayer::GeoCoordinateRole:
    {
      Nullable<QGeoCoordinate> coord =
          mPhotoList.at(index.row()).exifInfo().location;

      if (coord != nullptr)
        return QVariant::fromValue<QGeoCoordinate>(*coord);
      else
        return QVariant::fromValue<QGeoCoordinate>(QGeoCoordinate());
    }

    default:
      return QVariant();
  }
}

void PhotoModel::loadOriginal(Photo& photo)
{
  if (photo.isDownloadingOriginal())
    return;

  //    qDebug() << "PHOTOMODEL: load original";

  // cancel existing jobs and only load the last request.
  mOriginalThreadQueue->cancel();

  photo.setIsDownloadingOriginal(true);

  ImageLoaderJob* ilj = new ImageLoaderJob(photo, false);
  ilj->connect(ilj, &ImageLoaderJob::imageReady, this,
               &PhotoModel::onOriginalLoaded);
  ilj->connect(ilj, &ImageLoaderJob::exifUpdated, this,
               &PhotoModel::onExifUpdated);
  ilj->connect(ilj, &ImageLoaderJob::saveParams, this,
               &PhotoModel::onSaveParams);
  uint32_t id = mOriginalThreadQueue->addJob(ilj);
  mOriginalLoadThreads.insert(photo.id(), id);
}

void PhotoModel::loadPreview(Photo& photo)
{
  if (photo.isDownloadingPreview())
    return;

  //    qDebug() << "PHOTOMODEL: load preview";

  photo.setIsDownloadingPreview(true);

  ImageLoaderJob* ilj = new ImageLoaderJob(photo, true);
  ilj->connect(ilj, &ImageLoaderJob::previewReady, this,
               &PhotoModel::onPreviewLoaded);
  ilj->connect(ilj, &ImageLoaderJob::exifUpdated, this,
               &PhotoModel::onExifUpdated);
  ilj->connect(ilj, &ImageLoaderJob::saveParams, this,
               &PhotoModel::onSaveParams);
  uint32_t id = mPreviewThreadQueue->addJob(ilj);
  mPreviewLoadThreads.insert(photo.id(), id);
}

void PhotoModel::onExifUpdated(Photo photo)
{
  // get actual width x height & store in db.
  DatabaseAccess::photoDao()->updateExifInfo(photo);
}

void PhotoModel::onSaveParams(
    Photo photo, const QSharedPointer<DevelopRawParameters>& params)
{
  // save to DB
  DatabaseAccess::developSettingDao()->insertDefaultRawSettings(photo.id(),
                                                                params);
}

void PhotoModel::onPreviewLoaded(Photo photo, const QImage& image)
{
  if (!image.isNull())
  {
    photo.setLibraryPreview(image);
  }
  else
  {
    qDebug() << "Failed to load preview.. Prevent loop";
    photo.setLibraryPreview(QImage(":/images/loading_failed.jpg"));
  }
  mPreviewLoadThreads.remove(photo.id());
  photo.setIsDownloadingPreview(false);
  QVector<int> roles;
  roles << Photo::DataRole << MapView::ModelIndexLayer::DataRole;
  emit dataChanged(mPhotoIndexMap.value(photo.id()),
                   mPhotoIndexMap.value(photo.id()), roles);
}

void PhotoModel::onOriginalLoaded(Photo photo, const Image& image)
{
  if (!image.isNull())
  {
    photo.setOriginalImage(image);
  }
  else
  {
    qDebug() << "Failed to load original.. Not yet preventing loop";
  }
  photo.setIsDownloadingOriginal(false);
  mOriginalLoadThreads.remove(photo.id());
  QVector<int> roles;
  roles << Photo::DataRole << MapView::ModelIndexLayer::DataRole;
  emit dataChanged(mPhotoIndexMap.value(photo.id()),
                   mPhotoIndexMap.value(photo.id()), roles);
}

void PhotoModel::convertImage(Photo& photo)
{
  if (photo.isDownloadingPreview())
    return;

  photo.setIsDownloadingPreview(true);
  ColorTransformJob* cfj =
      new ColorTransformJob(photo, ColorTransformJob::Preview);

  // convert the image to sRGB
  cfj->connect(cfj, &ColorTransformJob::imageReady, this,
               &PhotoModel::onPreviewTranslated);
  uint32_t id = mPreviewThreadQueue->addJob(cfj);
  mPreviewLoadThreads.insert(photo.id(), id);
}

void PhotoModel::convertOriginal(Photo& photo)
{
  if (photo.isDownloadingOriginal())
    return;

  //    qDebug() << "PHOTOMODEL: converting original";

  photo.setIsDownloadingOriginal(true);
  ColorTransformJob* cfj =
      new ColorTransformJob(photo, ColorTransformJob::Develop);

  // convert the image to sRGB
  cfj->connect(cfj, &ColorTransformJob::imageReady, this,
               &PhotoModel::onOriginalTranslated);
  uint32_t id = mOriginalThreadQueue->addJob(cfj);
  mOriginalLoadThreads.insert(photo.id(), id);
}

void PhotoModel::onPreviewTranslated(Photo photo, const QImage& image)
{
  if (!image.isNull())
  {
    photo.setLibraryPreviewsRGB(image);

    // find the index for this photo.
    QVector<int> roles;
    roles << Photo::DataRole << MapView::ModelIndexLayer::DataRole;
    emit dataChanged(mPhotoIndexMap.value(photo.id()),
                     mPhotoIndexMap.value(photo.id()), roles);
  }
  mPreviewLoadThreads.remove(photo.id());
  photo.setIsDownloadingPreview(false);
}

void PhotoModel::onOriginalTranslated(Photo photo, const QImage& image)
{
  if (!image.isNull())
  {
    photo.setDevelopPreviewsRGB(image);

    // find the index for this photo.
    QVector<int> roles;
    roles << Photo::DataRole << MapView::ModelIndexLayer::DataRole;
    emit dataChanged(mPhotoIndexMap.value(photo.id()),
                     mPhotoIndexMap.value(photo.id()), roles);
  }
  mOriginalLoadThreads.remove(photo.id());
  photo.setIsDownloadingOriginal(false);
}

void PhotoModel::setRootSourceId(PhotoModel::SourceType source, long long id)
{
  beginResetModel();

  mRootId      = id;
  mPhotoSource = source;

  // cancel all running jobs
  mPreviewThreadQueue->cancel();

  if (source == SourceFiles)
  {
    mPhotoList = mWorkUnit->getPhotosByPath(
        id, Preferences::instance()->library_include_subfolders);
    mPhotoIndexMap.clear();
    int i = 0;
    foreach (Photo p, mPhotoList)
    {
      p.setOwner(this);
      mPhotoIndexMap.insert(p.id(), index(i));
      i++;
    }
  }
  else if (source == SourceCollectionUser || source == SourceCollectionImport ||
           source == SourceCollectionWork)
  {
    mPhotoList = mWorkUnit->getPhotosByCollectionId(
        id, Preferences::instance()->library_include_subfolders);
    mPhotoIndexMap.clear();
    int i = 0;
    foreach (Photo p, mPhotoList)
    {
      p.setOwner(this);
      mPhotoIndexMap.insert(p.id(), index(i));
      i++;
    }
  }
  // should also cancel all open threads
  endResetModel();
}

long long PhotoModel::rootId()
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
  //    Photo dragPhoto = data(index,
  //    Widgets::TileView::ImageRole).value<Photo>();

  //    image = QPixmap::fromImage(dragPhoto.libraryPreview().
  //            scaled(150, 150, Qt::KeepAspectRatio,
  //            Qt::SmoothTransformation));
  //    hotspot = QPoint(image.width() / 2, image.height() / 2);

  // get the id's of all selected items
  QList<long long> idlist;
  foreach (QModelIndex index, indexes)
  {
    Photo p = data(index, Photo::DataRole).value<Photo>();

    idlist << p.id();
  }

  DragDropInfo info(DragDropInfo::PhotoModel, idlist);
  mimeData->setData(MIMETYPE_TILEVIEW_SELECTION, info.toByteArray());
  return mimeData;
}

bool PhotoModel::canDropMimeData(const QMimeData* /*data*/,
                                 Qt::DropAction /*action*/, int /*row*/,
                                 int /*column*/,
                                 const QModelIndex& /*parent*/) const
{
  // do not support drops for now.
  return false;
}

bool PhotoModel::dropMimeData(const QMimeData* /*data*/,
                              Qt::DropAction /*action*/, int /*row*/,
                              int /*column*/, const QModelIndex& /*parent*/)
{
  return false;
}

PhotoModel::SourceType PhotoModel::rootSource() const
{
  return mPhotoSource;
}

void PhotoModel::onVisibleTilesChanged(const QModelIndex& start,
                                       const QModelIndex& end)
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
    uint32_t id = mPreviewLoadThreads.value(p.id());

    if (id >
        0) // if it is 0 it has already completed. 0 is not a valid thread id
      idList.append(id);
  }

  if (!idList.isEmpty())
  {
    // then purge all jobs but keep the id's in the list.
    mPreviewThreadQueue->purgeExcept(idList);
  }
}

void PhotoModel::onPhotosAdded(long long /*pathid*/,
                               const QList<long long>& idList)
{
  int start = rowCount();

  QList<Photo> list = mWorkUnit->getPhotosById(idList);

  beginInsertRows(QModelIndex(), start, start);
  foreach (Photo info, list)
  {
    info.setOwner(this);
    mPhotoList.append(info);
  }
  endInsertRows();
}

void PhotoModel::onPhotosDeletedFromCollection(long long           collectionid,
                                               const QList<Photo>& photos)
{
  if (mPhotoSource == SourceCollectionUser && collectionid == mRootId)
    onPhotosDeleted(photos);
}

void PhotoModel::onPhotosDeleted(const QList<Photo>& photos)
{
  foreach (Photo photo, photos)
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
  foreach (Photo photo, photos)
  {
    int count = mPhotoList.size();
    int i;

    for (i = 0; i < count; i++)
      if (photo.id() == mPhotoList.at(i).id())
        emit dataChanged(index(i), index(i));
  }
}

void PhotoModel::refreshData(const QList<Photo>& /*list*/)
{
  // for now just emit that all data has changed. the tileview doesnt check
  // anyway
  emit dataChanged(index(0), index(rowCount() - 1));
}

bool PhotoModel::removeRows(int row, int count, const QModelIndex& parent)
{
  if (row >= mPhotoList.size() || row + count > mPhotoList.size() || row < 0 ||
      count < 0)
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

  foreach (QModelIndex index, selection.indexes())
  {
    list.append(mPhotoList.at(index.row()));
  }
  return list;
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
} // namespace PhotoStage
