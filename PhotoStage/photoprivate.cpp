#include <QDir>

#include <QDebug>

#include "database/dbutils.h"
#include "photo.h"

namespace PhotoStage {
Photo::PhotoPrivate::PhotoPrivate(PhotoOwner* owner, const QImage& image,
                                  const QString& filename, long long id)
    : mId(id), mLibraryPreview(image), mSrcImagePath(filename), mOwner(owner),
      mIsDownloadingPreview(false), mIsDownloadingOriginal(false),
      mPhotoType(ContainerInvalid), mDevelopHistoryId(-1)
{
}

Photo::PhotoPrivate::PhotoPrivate(QSqlQuery& q)
    : mOwner(nullptr), mIsDownloadingPreview(false),
      mIsDownloadingOriginal(false), mPhotoType(ContainerInvalid),
      mDevelopHistoryId(-1)
{
  mId              = q.value(0).toLongLong();
  QString filename = q.value(1).toString();
  QString path     = q.value(2).toString();
  mSrcImagePath    = path + QDir::separator() + filename;

  mRating = q.value(3).isNull() ? 0 : q.value(3).toInt();

  mColorLabel = q.value(4).isNull() ? Photo::LabelNoColor
                                    : (Photo::ColorLabel)q.value(4).toInt();
  mFlag =
      q.value(5).isNull() ? Photo::FlagNone : (Photo::Flag)q.value(5).toInt();

  // p.id, p.filename, c.directory,p.rating,p.color,p.flag,
  // p.iso, p.aperture, p.exposure_time, p.focal_length, p.datetime_original,
  // p.datetime_digitized, p.rotatation, p.latitude, p.longitude,
  // p.copyright, p.artist, p.flash, p.lens_name, p.make, p.model
  getNullableValue(q, 6, mExifInfo.isoSpeed);
  getNullableValue(q, 7, mExifInfo.aperture);
  getNullableValue(q, 8, mExifInfo.exposureTime);
  getNullableValue(q, 9, mExifInfo.focalLength);
  getNullableValue(q, 10, mExifInfo.dateTimeOriginal);
  getNullableValue(q, 11, mExifInfo.dateTimeDigitized);
  mExifInfo.rotation = q.value(12).isNull()
                           ? ExifInfo::NotRotated
                           : (ExifInfo::Rotation)q.value(12).toInt();

  if (!q.value(13).isNull() && !q.value(14).isNull())
    mExifInfo.location =
        QGeoCoordinate(q.value(13).toDouble(), q.value(14).toDouble());
  else
    mExifInfo.location = nullptr;
  getNullableValue(q, 15, mExifInfo.copyright);
  getNullableValue(q, 16, mExifInfo.artist);
  getNullableValue(q, 17, mExifInfo.flash);
  getNullableValue(q, 18, mExifInfo.lensName);
  getNullableValue(q, 19, mExifInfo.make);
  getNullableValue(q, 20, mExifInfo.model);
  // these two values are declared NOT NULL
  mExifInfo.width  = q.value(21).toInt();
  mExifInfo.height = q.value(22).toInt();

  QString type = q.value(23).toString();

  if (type == "RAW")
    mPhotoType = ContainerRAW;
  else if (type == "JPG")
    mPhotoType = ContainerJPG;
  else if (type == "PNG")
    mPhotoType = ContainerPNG;

  mExifInfo.profileName = q.value(24).toString();

  getNullableValue(q, 25, mDevelopHistoryId);
}

Photo::PhotoPrivate::~PhotoPrivate()
{
}
} // namespace PhotoStage
