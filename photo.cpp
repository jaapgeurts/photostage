#include <QDebug>

#include "photo.h"
#include "photomodel.h"

namespace PhotoStage
{
Photo::Photo()
{
}

Photo::Photo(PhotoOwner *owner, const QImage& image, const QString& filename, long long id)
{
    d = QSharedPointer<PhotoPrivate>(new PhotoPrivate(owner, image, filename, id));
}

Photo::Photo(QSqlQuery& q)
{
    d = QSharedPointer<PhotoPrivate>(new PhotoPrivate(q));
}

long long Photo::id() const
{
    return d->mId;
}

void Photo::setOriginalImage(const Image& image)
{
    d->mOriginal = image;
}

const Image& Photo::originalImage()
{
    const Image& img = d->mOriginal;

    if (img.isNull())
    {
        d->mOwner->loadOriginal(*this);
    }
    return img;
}

void Photo::setLibraryPreview(const QImage& image)
{
    d->mLibraryPreview     = image;
    d->mLibraryPreviewsRGB = QImage();
}

void Photo::setLibraryPreviewsRGB(const QImage& image)
{
    d->mLibraryPreviewsRGB = image;
}

void Photo::setDevelopPreviewsRGB(const QImage& image)
{
    d->mDevelopPreviewsRGB = image;
}

const QImage& Photo::libraryPreview()
{
    const QImage& img = d->mLibraryPreview;

    if (img.isNull())
    {
        d->mOwner->loadPreview(*this);
    }
    return img;
}

const QImage& Photo::libraryPreviewsRGB()
{
    // make sure the library preview is avaialble,
    // if it is not trigger download

    if (d->mLibraryPreviewsRGB.isNull())
    {
        if (d->mLibraryPreview.isNull())
            libraryPreview();
        else
            d->mOwner->convertImage(*this);
    }

    return d->mLibraryPreviewsRGB;
}

const QImage& Photo::developPreviewsRGB()
{
    // make sure the original is avaialble,
    // if it is not trigger download

    if (d->mDevelopPreviewsRGB.isNull())
    {
        if (d->mOriginal.isNull())
            originalImage();
        else
            d->mOwner->convertOriginal(*this);
    }

    return d->mDevelopPreviewsRGB;
}

void Photo::setSrcImagePath(const QString& path)
{
    d->mSrcImagePath = path;
}

const QString& Photo::srcImagePath() const
{
    return d->mSrcImagePath;
}

void Photo::setExifInfo(const ExifInfo& exifInfo)
{
    d->mExifInfo = exifInfo;
}

ExifInfo& Photo::exifInfo()
{
    return d->mExifInfo;
}

const ExifInfo& Photo::exifInfo() const
{
    return d->mExifInfo;
}

void Photo::setRating(int rating)
{
    if (rating < 0 || rating > 5)
    {
        qDebug() << "Rating out of range. Must be between 0<=rating<=5";
        return;
    }
    d->mRating = rating;
}

int Photo::rating() const
{
    return d->mRating;
}

void Photo::setColorLabel(ColorLabel label)
{
    d->mColorLabel = label;
}

Photo::ColorLabel Photo::colorLabel() const
{
    return d->mColorLabel;
}

void Photo::setFlag(Flag flag)
{
    d->mFlag = flag;
}

Photo::Flag Photo::flag() const
{
    return d->mFlag;
}

bool Photo::isRaw() const
{
    return d->mIsRaw;
}

void Photo::setIsRaw(bool isRaw)
{
    d->mIsRaw = isRaw;
}

long long Photo::hash() const
{
    return d->mHashCode;
}

void Photo::setHash(long long code)
{
    d->mHashCode = code;
}

bool Photo::isNull() const
{
    return d.isNull();
}

void Photo::setIsDownloadingOriginal(bool value)
{
    d->mIsDownloadingOriginal = value;
}

bool Photo::isDownloadingOriginal() const

{
    return d->mIsDownloadingOriginal;
}

void Photo::setIsDownloadingPreview(bool value)
{
    d->mIsDownloadingPreview = value;
}

bool Photo::isDownloadingPreview() const
{
    return d->mIsDownloadingPreview;
}

void Photo::setKeywords(const QStringList& list)
{
    d->mKeywords = list;
}

QStringList Photo::keywords() const
{
    return d->mKeywords;
}

void Photo::setOwner(PhotoOwner* owner)
{
    d->mOwner = owner;
}
}
