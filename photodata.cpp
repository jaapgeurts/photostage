#include <QDebug>
#include <QVariant>
#include <QDir>

#include "engine/colortransform.h"

#include "photomodel.h"
#include "photodata.h"

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

    if (q.value(3).isNull())
        setRating(0);
    else
        setRating(q.value(3).toInt());
    setColorLabel((Photo::ColorLabel)q.value(4).toInt());
    setFlag((Photo::Flag)q.value(5).toInt());

    // p.id, p.filename, c.directory,p.rating,p.color,p.flag, \
    // p.iso, p.aperture, p.exposure_time, p.focal_length, p.datetime_original,\
    // p.datetime_digitized, p.rotatation, p.lattitude, p.longitude, \
    // p.copyright, p.artist, p.flash, p.lens_name, p.make, p.model
    mExifInfo.isoSpeed          = q.value(6).toInt();
    mExifInfo.aperture          = q.value(7).toInt();
    mExifInfo.exposureTime      = q.value(8).toFloat();
    mExifInfo.focalLength       = q.value(9).toFloat();
    mExifInfo.dateTimeOriginal  = q.value(10).toDateTime();
    mExifInfo.dateTimeDigitized = q.value(11).toDateTime();
    mExifInfo.rotation          = (ExifInfo::Rotation)q.value(12).toInt();
    mExifInfo.location          = QGeoCoordinate(q.value(13)
            .toDouble(), q.value(14).toDouble());
    mExifInfo.copyright = q.value(15).toString();
    mExifInfo.artist    = q.value(16).toString();
    mExifInfo.flash     = q.value(17).toBool();
    mExifInfo.lensName  = q.value(18).toString();
    mExifInfo.make      = q.value(19).toString();
    mExifInfo.model     = q.value(20).toString();
    mExifInfo.width     = q.value(21).toInt();
    mExifInfo.height    = q.value(22).toInt();
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

// TODO: think about using the proxy pattern
const QImage& PhotoData::libraryPreview()
{
    return mLibraryPreview;
}

const QImage& PhotoData::libraryPreviewsRGB()
{
    if (mLibraryPreviewsRGB.isNull())
    {
        // run this in a thread so the UI is fast.

        // convert the image to sRGB
        //qDebug() << "Converting image to RGB";
        ColorTransform transform = ColorTransform::getTransform(
            "Melissa-sRGB-RGB32",
            WORKING_COLOR_SPACE,
            ColorTransform::getMonitorProfilePath(),
            ColorTransform::FORMAT_RGB32,
            ColorTransform::FORMAT_RGB32);
        mLibraryPreviewsRGB = transform.transformQImage(mLibraryPreview);
    }
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
}
