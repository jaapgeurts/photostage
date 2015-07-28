#include <QDebug>
#include <QVariant>
#include <QDir>

#include "engine/colortransform.h"

#include "photomodel.h"
#include "photodata.h"

#include "database/dbutils.h"
#include "utils.h"

namespace PhotoStage
{
PhotoData::PhotoData(PhotoModel* owner,
    const QImage& image,
    const QString& filename,
    long long id) :
    mId(id),
    mLibraryPreview(image),
    mSrcImagePath(filename),
    mOwner(owner),
    mIsDownloading(false)
{
}

PhotoData::PhotoData(QSqlQuery& q) :
    mOwner(NULL),
    mIsDownloading(false)
{
    mId = q.value(0).toLongLong();
    QString filename = q.value(1).toString();
    QString path     = q.value(2).toString();
    mSrcImagePath = path + QDir::separator() + filename;

    mRating = q.value(3).isNull() ?  0 : q.value(3).toInt();

    mColorLabel = q.value(4).isNull() ? Photo::LabelNoColor : (Photo::ColorLabel)q.value(4).toInt();
    mFlag       = q.value(5).isNull() ? Photo::FlagNone : (Photo::Flag)q.value(5).toInt();

    // p.id, p.filename, c.directory,p.rating,p.color,p.flag, \
    // p.iso, p.aperture, p.exposure_time, p.focal_length, p.datetime_original,\
    // p.datetime_digitized, p.rotatation, p.lattitude, p.longitude, \
    // p.copyright, p.artist, p.flash, p.lens_name, p.make, p.model
    getDbValue(q, 6, mExifInfo.isoSpeed);
    getDbValue(q, 7, mExifInfo.aperture);
    getDbValue(q, 8, mExifInfo.exposureTime);
    getDbValue(q, 9, mExifInfo.focalLength);
    getDbValue(q, 10, mExifInfo.dateTimeOriginal);
    getDbValue(q, 11, mExifInfo.dateTimeDigitized);
    mExifInfo.rotation = q.value(12).isNull() ? ExifInfo::NotRotated : (ExifInfo::Rotation)q.value(12).toInt();

    if (!q.value(13).isNull() && !q.value(14).isNull())
        mExifInfo.location =  QGeoCoordinate(q.value(13).toDouble(), q.value(14).toDouble());
    else
        mExifInfo.location = nullptr;
    getDbValue(q, 15, mExifInfo.copyright);
    getDbValue(q, 16, mExifInfo.artist);
    getDbValue(q, 17, mExifInfo.flash);
    getDbValue(q, 18, mExifInfo.lensName );
    getDbValue(q, 19, mExifInfo.make );
    getDbValue(q, 20, mExifInfo.model);
    // these two values are declared NOT NULL
    mExifInfo.width  = q.value(21).toInt();
    mExifInfo.height = q.value(22).toInt();
}

PhotoData::~PhotoData()
{
}

void PhotoData::setOriginal(const QImage& image)
{
    mOriginal = image;
}

const QImage& PhotoData::original() const
{
    return mOriginal;
}

void PhotoData::setLibraryPreview(const QImage& image)
{
    mLibraryPreview = image;
    // force regeneration of the display image
    mLibraryPreviewsRGB = QImage();
}

void PhotoData::setLibraryPreviewsRGB(const QImage& image)
{
    mLibraryPreviewsRGB = image;
}

// TODO: think about using the proxy pattern
const QImage& PhotoData::libraryPreview()
{
    return mLibraryPreview;
}

const QImage& PhotoData::libraryPreviewsRGB()
{
    return mLibraryPreviewsRGB;
}

void PhotoData::setSrcImagePath(const QString& path)
{
    mSrcImagePath = path;
}

const QString& PhotoData::srcImagePath() const
{
    return mSrcImagePath;
}

void PhotoData::setExifInfo(const ExifInfo& exifInfo)
{
    mExifInfo = exifInfo;
}

ExifInfo& PhotoData::exifInfo()
{
    return mExifInfo;
}

const ExifInfo& PhotoData::exifInfo() const
{
    return mExifInfo;
}

void PhotoData::setRating(int rating)
{
    if (rating < 0 || rating > 5)
    {
        qDebug() << "Invalid rating:" << rating;
        return;
    }
    mRating = rating;
}

int PhotoData::rating() const
{
    return mRating;
}

void PhotoData::setColorLabel(Photo::ColorLabel label)
{
    mColorLabel = label;
}

Photo::ColorLabel PhotoData::colorLabel() const
{
    return mColorLabel;
}

void PhotoData::setFlag(Photo::Flag flag)
{
    mFlag = flag;
}

Photo::Flag PhotoData::flag() const
{
    return mFlag;
}

long long PhotoData::hash() const
{
    return mHashCode;
}

void PhotoData::setHash(long long code)
{
    mHashCode = code;
}

long long PhotoData::id() const
{
    return mId;
}

PhotoModel* PhotoData::owner() const
{
    return mOwner;
}

void PhotoData::setOwner(PhotoModel* owner)
{
    mOwner = owner;
}

void PhotoData::setIsDownloading(bool value)
{
    mIsDownloading = value;
}

bool PhotoData::isDownloading() const
{
    return mIsDownloading;
}

void PhotoData::setKeywords(const QStringList& list)
{
    mKeywords = list;
}

QStringList PhotoData::keywords() const
{
    return mKeywords;
}
}
