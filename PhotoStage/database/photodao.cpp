#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "databaseaccess.h"
#include "dbutils.h"
#include "photodao.h"
#include "previewcache.h"
#include "utils.h"

namespace PhotoStage {
PhotoDAO::PhotoDAO(QObject* parent) : QObject(parent)
{
}

void PhotoDAO::setRating(const QList<Photo>& list, int rating)
{
  if (list.isEmpty())
    return;

  QSqlQuery q;
  QString   query = "update photo set rating=:rating where id in (:id)";

  QString ids = joinIds(list);

  query.replace(":id", ids);
  q.prepare(query);
  q.bindValue(":rating", rating);
  q.exec();

  foreach (Photo p, list)
    p.setRating(rating);
  emit photosChanged(list);
}

void PhotoDAO::setFlag(const QList<Photo>& list, Photo::Flag flag)
{
  if (list.isEmpty())
    return;

  QSqlQuery q;
  QString   query = ("update photo set flag=:flag where id in (:id)");

  QString ids = joinIds(list);
  query.replace(":id", ids);
  q.prepare(query);
  q.bindValue(":flag", (int)flag);
  q.exec();
  foreach (Photo p, list)
    p.setFlag(flag);

  emit photosChanged(list);
}

void PhotoDAO::setColorLabel(const QList<Photo>& list, Photo::ColorLabel color)
{
  if (list.isEmpty())
    return;

  QSqlQuery q;
  QString   query = "update photo set color=:color where id in (:id)";

  QString ids = joinIds(list);

  query.replace(":id", ids);
  q.prepare(query);
  q.bindValue(":color", (int)color);
  q.exec();
  foreach (Photo p, list)
    p.setColorLabel(color);

  emit photosChanged(list);
}

void PhotoDAO::assignGeoCoordinate(const QGeoCoordinate& coord,
                                   const QList<Photo>&   list)
{
  if (list.isEmpty())
    return;

  QSqlQuery q;
  QString   query = "update photo set longitude=:longitude, latitude=:latitude "
                  "where id in (:id)";

  QString ids = joinIds(list);

  query.replace(":id", ids);
  q.prepare(query);
  q.bindValue(":longitude", coord.longitude());
  q.bindValue(":latitude", coord.latitude());

  if (!q.exec())
  {
    qDebug() << "Can't update photo";
    qDebug() << q.lastError();
  }
  foreach (Photo p, list)
    p.exifInfo().location = coord;

  emit photosChanged(list);
}

void PhotoDAO::importPhoto(long long collectionid, int pathId,
                           const QString& fileName, long long hash,
                           const ExifInfo& exifInfo)
{

  // send this to the DAO
  QSqlQuery q;
  q.prepare(
      "insert into photo (path_id,filename,photo_hash, iso, exposure_time, \
        focal_length, datetime_original, datetime_digitized, rotation, \
        longitude, latitude, copyright, artist, aperture, flash, lens_name,  \
        make, model, width, height ) \
      values (:path, :filename,:hash,:iso,:exposure_time,:focal_length,\
              :datetime_original,:datetime_digitized,:rotation,:longitude, \
              :latitude,:copyright,:artist,:aperture,:flash,:lens_name, \
              :make, :model, :width, :height)");
  q.bindValue(":path", pathId);
  q.bindValue(":filename", fileName);
  q.bindValue(":hash", hash);
  q.bindValue(":iso", setDbValue(exifInfo.isoSpeed));
  q.bindValue(":exposure_time", setDbValue(exifInfo.exposureTime));
  q.bindValue(":focal_length", setDbValue(exifInfo.focalLength));
  q.bindValue(":datetime_original", setDbValue(exifInfo.dateTimeOriginal));
  q.bindValue(":datetime_digitized", setDbValue(exifInfo.dateTimeDigitized));
  q.bindValue(":rotation", exifInfo.rotation);

  if (exifInfo.location != nullptr)
  {
    q.bindValue(":longitude", exifInfo.location->longitude());
    q.bindValue(":latitude", exifInfo.location->latitude());
  }
  else
  {
    q.bindValue(":longitude", QVariant(QVariant::Double));
    q.bindValue(":latitude", QVariant(QVariant::Double));
  }
  q.bindValue(":copyright", setDbValue(exifInfo.copyright));
  q.bindValue(":artist", setDbValue(exifInfo.artist));
  q.bindValue(":aperture", setDbValue(exifInfo.aperture));
  q.bindValue(":flash", setDbValue(exifInfo.flash));
  q.bindValue(":lens_name", setDbValue(exifInfo.lensName));
  q.bindValue(":make", setDbValue(exifInfo.make));
  q.bindValue(":model", setDbValue(exifInfo.model));
  q.bindValue(":width", exifInfo.width);
  q.bindValue(":height", exifInfo.height);

  bool result = q.exec();

  long long ret = -1;
  if (!result)
  {
    qDebug() << "Can't insert" << fileName << "into DB";
    qDebug() << q.lastError();
  }
  else
  {
    ret = q.lastInsertId().toLongLong();
    // make sure the global preview cache does not store an image with the
    // same key this is probably a left over cache entry from before.
    PreviewCache::globalCache()->remove(QString::number(ret));
  }

  if (ret != -1)
  {
    QList<long long> list;
    list << ret;
    DatabaseAccess::collectionDao()->addPhotosToCollection(collectionid, list);
    emit photosAdded(pathId, list);
  }
}

QList<Photo> PhotoDAO::getPhotosById(const QList<long long> idList) const
{
  QSqlQuery q;
  QString   query =
      QString("select p.id, p.filename, d.path ,p.rating,p.color,p.flag, \
        p.iso, p.aperture,p.exposure_time, p.focal_length, p.datetime_original, \
        p.datetime_digitized, p.rotation, p.latitude,p.longitude, \
        p.copyright, p.artist, p.flash, p.lens_name, p.make,  p.model, \
        p.width, p.height, p.photo_type, p.color_profile_name, p.develop_history_id \
        from photo p join (select group_concat(ancestor.directory ,:separator) as path, \
                           child.* from path child join path ancestor \
                           on child.lft >= ancestor.lft \
                           and child.lft <= ancestor.rgt \
                           group by child.lft \
                           order by child.lft) as d on p.path_id = d.id \
      and p.id in (:photoids);");

  QString photoids = joinIds(idList);

  query.replace(":photoids", photoids);
  q.prepare(query);
  q.bindValue(":separator", QDir::separator());

  QList<Photo> list;

  if (!q.exec() && !q.isValid())
  {
    qDebug() << "Query failed" << q.executedQuery();
    qDebug() << q.lastError();
    return list;
  }

  while (q.next())
  {
    Photo p(q);
    p.setKeywords(DatabaseAccess::keywordDao()->getPhotoKeywords(p));
    list.append(p);
  }
  return list;
}

void PhotoDAO::deletePhotos(const QList<Photo>& list, bool deleteFile)
{
  if (list.isEmpty())
    return;

  QSqlQuery q;
  QString   photo_ids;

  // create a list of id's
  QList<QString> paths;

  foreach (Photo info, list)
  {
    photo_ids += QString::number(info.id()) + ",";
    paths << info.srcImagePath();
  }
  photo_ids.chop(1);

  // first delete any keyword references
  QString query = "delete from photo_keyword where photo_id in (:photo_ids)";
  query.replace(":photo_ids", photo_ids);

  if (!q.exec(query))
  {
    qDebug() << q.lastError();
    qDebug() << q.lastQuery();
    return;
  }

  // also remove any references in all collections.

  // now delete the photos from the main library.
  query = "delete from photo where id in (:photo_ids)";
  query.replace(":photo_ids", photo_ids);

  if (!q.exec(query))
  {
    qDebug() << q.lastError();
    qDebug() << q.lastQuery();
    return;
  }

  // Finally delete the actual file
  if (deleteFile)
  {
    foreach (QString path, paths)
    {
      QFile f(path);

      if (!f.remove())
        qDebug() << "File" << path << "could not be removed";
    }
  }
  emit photosDeleted(list);
}

void PhotoDAO::getLeftRightForCollectionId(long long  collection_id,
                                           long long& lft, long long& rgt) const
{
  QSqlQuery q;

  q.prepare("select lft,rgt from collection where id = :id;");
  q.bindValue(":id", collection_id);

  if (!q.exec() && !q.isValid())
  {
    qDebug() << "Query failed" << q.executedQuery();
    qDebug() << q.lastError();
    return;
  }
  q.next();

  lft = q.value(0).toLongLong();
  rgt = q.value(1).toLongLong();
}

void PhotoDAO::getLeftRightForPathId(long long path_id, long long& lft,
                                     long long& rgt) const
{
  QSqlQuery q;

  q.prepare("select lft,rgt from path where id = :id;");
  q.bindValue(":id", path_id);

  if (!q.exec() && !q.isValid())
  {
    qDebug() << "Query failed" << q.executedQuery();
    qDebug() << q.lastError();
    return;
  }
  q.next();

  lft = q.value(0).toLongLong();
  rgt = q.value(1).toLongLong();
}

/**
 * @brief PhotoWorkUnit::getPhotosByPath Return a list of photo's that are in
 * the path identified by path id. If includeSubDirs is true, then all photos
 * under this path are returned as well.
 * @param path_id
 * @param includeSubDirs
 * @return
 */
QList<Photo> PhotoDAO::getPhotosByPath(long long path_id,
                                       bool      includeSubDirs) const
{
  QSqlQuery q;

  QList<Photo> list;
  q.clear();

  if (includeSubDirs)
  {
    long long lft, rgt;
    getLeftRightForPathId(path_id, lft, rgt);

    q.prepare("select p.id, p.filename, d.path,p.rating,p.color,p.flag, \
          p.iso, p.aperture,p.exposure_time, p.focal_length, p.datetime_original, \
          p.datetime_digitized, p.rotation, p.latitude,p.longitude, \
          p.copyright, p.artist, p.flash, p.lens_name, p.make,  p.model, \
          p.width, p.height, p.photo_type, p.color_profile_name, p.develop_history_id  \
          from photo p join (select group_concat(ancestor.directory ,:separator) as path, \
                             child.* from path child join path ancestor \
                             on child.lft >= ancestor.lft \
                             and child.lft <= ancestor.rgt \
                             group by child.lft \
                             order by child.lft) as d on p.path_id = d.id \
        and d.lft between :lft and :rgt;");
    q.bindValue(":lft", lft);
    q.bindValue(":rgt", rgt);
  }
  else
  {
    q.prepare("select p.id, p.filename, d.path,p.rating,p.color,p.flag, \
          p.iso, p.aperture,p.exposure_time, p.focal_length, p.datetime_original, \
          p.datetime_digitized, p.rotation, p.latitude,p.longitude, \
          p.copyright, p.artist, p.flash, p.lens_name, p.make,  p.model, \
          p.width, p.height, p.photo_type, p.color_profile_name, p.develop_history_id \
          from photo p join (select group_concat(ancestor.directory ,:separator) as path, \
                             child.* from path child join path ancestor \
                             on child.lft >= ancestor.lft \
                             and child.lft <= ancestor.rgt \
                             group by child.lft \
                             order by child.lft) as d on p.path_id = d.id \
        and p.path_id = :pathid;");
    q.bindValue(":pathid", path_id);
  }
  q.bindValue(":separator", QDir::separator());

  if (!q.exec() && !q.isValid())
  {
    qDebug() << "Query failed" << q.executedQuery();
    qDebug() << q.lastError();
    return list;
  }

  while (q.next())
  {
    Photo p(q);
    p.setKeywords(DatabaseAccess::keywordDao()->getPhotoKeywords(p));
    list.append(p);
  }
  return list;
}

QList<Photo> PhotoDAO::getPhotosByCollectionId(long long collection_id,
                                               bool      includeSubDirs) const
{
  QSqlQuery q;

  QList<Photo> list;
  q.clear();

  if (includeSubDirs)
  {
    long long lft, rgt;
    getLeftRightForCollectionId(collection_id, lft, rgt);

    // FIXME: this is not correct.
    q.prepare("select p.id, p.filename, d.path,p.rating,p.color,p.flag, \
          p.iso, p.aperture,p.exposure_time, p.focal_length, p.datetime_original, \
          p.datetime_digitized, p.rotation, p.latitude,p.longitude, \
          p.copyright, p.artist, p.flash, p.lens_name, p.make,  p.model, \
          p.width, p.height, p.photo_type, p.color_profile_name, p.develop_history_id  \
          from photo p join (select group_concat(ancestor.directory ,:separator) as path, \
                             child.* from path child join path ancestor \
                             on child.lft >= ancestor.lft \
                             and child.lft <= ancestor.rgt \
                             group by child.lft \
                             order by child.lft) as d on p.path_id = d.id \
        join collection_photo cp on p.id =  cp.photo_id \
        join collection c on cp.collection_id = c.id \
        and c.lft between :lft and :rgt;");
    q.bindValue(":lft", lft);
    q.bindValue(":rgt", rgt);
  }
  else
  {
    q.prepare("select p.id, p.filename, d.path,p.rating,p.color,p.flag, \
          p.iso, p.aperture,p.exposure_time, p.focal_length, p.datetime_original, \
          p.datetime_digitized, p.rotation, p.latitude,p.longitude, \
          p.copyright, p.artist, p.flash, p.lens_name, p.make,  p.model, \
          p.width, p.height, p.photo_type, p.color_profile_name, p.develop_history_id  \
          from photo p join (select group_concat(ancestor.directory ,:separator) as path, \
                             child.* from path child join path ancestor \
                             on child.lft >= ancestor.lft \
                             and child.lft <= ancestor.rgt \
                             group by child.lft \
                             order by child.lft) as d on p.path_id = d.id \
        and cp.collection_id = :collectionid \
        join collection_photo cp on p.id =  cp.photo_id;");
    q.bindValue(":collectionid", collection_id);
  }
  q.bindValue(":separator", QDir::separator());

  if (!q.exec() && !q.isValid())
  {
    qDebug() << "Query failed" << q.executedQuery();
    qDebug() << q.lastError();
    return list;
  }

  while (q.next())
  {
    Photo p(q);
    p.setKeywords(DatabaseAccess::keywordDao()->getPhotoKeywords(p));
    list.append(p);
  }
  return list;
}

void PhotoDAO::updateExifInfo(Photo& photo)
{
  QSqlQuery q;

  const ExifInfo& ei = photo.exifInfo();

  q.prepare(
      "update photo set width=:width, height=:height, photo_type=:photo_type,"
      " color_profile_name=:profile_name where id = :photoid");
  q.bindValue(":width", ei.width);
  q.bindValue(":height", ei.height);

  switch (photo.photoType())
  {
    case Photo::ContainerRAW:
      q.bindValue(":photo_type", "RAW");
      break;

    case Photo::ContainerJPG:
      q.bindValue(":photo_type", "JPG");
      break;

    case Photo::ContainerPNG:
      q.bindValue(":photo_type", "PNG");
      break;

    default:
      q.bindValue(":photo_type", QVariant(QVariant::String));
      break;
  }

  q.bindValue(":profile_name", ei.profileName);
  q.bindValue(":photoid", photo.id());

  if (!q.exec())
  {
    qDebug() << "Query failed" << q.executedQuery();
    qDebug() << q.lastError();
  }

  QList<Photo> list;
  list << photo;
  emit photosChanged(list);
}

void PhotoDAO::regenerateHash(Photo& photo)
{
  QString path = photo.srcImagePath();
  // only scan the first 1MB
  long long hash = computeImageFileHash(path);

  photo.setHash(hash);

  QSqlQuery q;
  q.prepare("update photo set photo_hash=:hash where id = :photoid");
  q.bindValue(":hash", photo.hash());
  q.bindValue(":photoid", photo.id());

  if (!q.exec())
  {
    qDebug() << "Query failed" << q.executedQuery();
    qDebug() << q.lastError();
  }

  QList<Photo> list;
  list << photo;
  emit photosChanged(list);
}

bool PhotoDAO::IsInLibrary(long long hash) const
{
  QSqlQuery q;

  q.prepare("select count(*) from photo where photo_hash = :hash");
  q.bindValue(":hash", hash);

  if (!q.exec())
  {
    qDebug() << "Query failed" << q.executedQuery();
    qDebug() << q.lastError();
  }

  if (!q.next())
    qDebug() << "select count(*) for hash returned no results!??";
  int res = q.value(0).toInt();

  if (res > 1)
    qDebug() << "There are two photos with the same hash";
  return res == 1;
}
} // namespace PhotoStage
