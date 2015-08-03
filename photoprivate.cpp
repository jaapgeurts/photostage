#include <QDir>

#include <QDebug>

#include "database/dbutils.h"
#include "photo.h"

namespace PhotoStage
{
Photo::PhotoPrivate::PhotoPrivate(PhotoModel* owner,
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

Photo::PhotoPrivate::PhotoPrivate(QSqlQuery& q) :
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

Photo::PhotoPrivate::~PhotoPrivate()
{
}

void Photo::PhotoPrivate::setOriginal(const QImage& image)
{
    mOriginal = image;
}

const QImage& Photo::PhotoPrivate::original() const
{
    return mOriginal;
}

void Photo::PhotoPrivate::setLibraryPreview(const QImage& image)
{
    mLibraryPreview = image;
    // force regeneration of the display image
    mLibraryPreviewsRGB = QImage();
}

void Photo::PhotoPrivate::setLibraryPreviewsRGB(const QImage& image)
{
    mLibraryPreviewsRGB = image;
}

// TODO: think about using the proxy pattern
const QImage& Photo::PhotoPrivate::libraryPreview()
{
    return mLibraryPreview;
}

const QImage& Photo::PhotoPrivate::libraryPreviewsRGB()
{
    return mLibraryPreviewsRGB;
}

void Photo::PhotoPrivate::setSrcImagePath(const QString& path)
{
    mSrcImagePath = path;
}

const QString& Photo::PhotoPrivate::srcImagePath() const
{
    return mSrcImagePath;
}

void Photo::PhotoPrivate::setExifInfo(const ExifInfo& exifInfo)
{
    mExifInfo = exifInfo;
}

ExifInfo& Photo::PhotoPrivate::exifInfo()
{
    return mExifInfo;
}

const ExifInfo& Photo::PhotoPrivate::exifInfo() const
{
    return mExifInfo;
}

void Photo::PhotoPrivate::setRating(int rating)
{
    if (rating < 0 || rating > 5)
    {
        qDebug() << "Invalid rating:" << rating;
        return;
    }
    mRating = rating;
}

int Photo::PhotoPrivate::rating() const
{
    return mRating;
}

void Photo::PhotoPrivate::setColorLabel(Photo::ColorLabel label)
{
    mColorLabel = label;
}

Photo::ColorLabel Photo::PhotoPrivate::colorLabel() const
{
    return mColorLabel;
}

void Photo::PhotoPrivate::setFlag(Photo::Flag flag)
{
    mFlag = flag;
}

Photo::Flag Photo::PhotoPrivate::flag() const
{
    return mFlag;
}

long long Photo::PhotoPrivate::hash() const
{
    return mHashCode;
}

void Photo::PhotoPrivate::setHash(long long code)
{
    mHashCode = code;
}

long long Photo::PhotoPrivate::id() const
{
    return mId;
}

PhotoModel* Photo::PhotoPrivate::owner() const
{
    return mOwner;
}

void Photo::PhotoPrivate::setOwner(PhotoModel* owner)
{
    mOwner = owner;
}

void Photo::PhotoPrivate::setIsDownloading(bool value)
{
    mIsDownloading = value;
}

bool Photo::PhotoPrivate::isDownloading() const
{
    return mIsDownloading;
}

void Photo::PhotoPrivate::setKeywords(const QStringList& list)
{
    mKeywords = list;
}

QStringList Photo::PhotoPrivate::keywords() const
{
    return mKeywords;
}
}
