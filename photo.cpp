#include <QDebug>

#include "photo.h"
#include "photodata.h"
#include "photomodel.h"

namespace PhotoStage
{
Photo::Photo()
{
}

Photo::Photo(PhotoModel* owner,
    const QImage& image,
    const QString& filename,
    long long id)
{
    d = QSharedPointer<PhotoData>(new PhotoData(owner, image, filename, id));
}

Photo::Photo(QSqlQuery& q)
{
    d = QSharedPointer<PhotoData>(new PhotoData(q));
}

long long Photo::id() const
{
    return d->id();
}

void Photo::setOriginal(const QImage& image)
{
    d->setOriginal(image);
}

const QImage& Photo::original() const
{
    return d->original();
}

void Photo::setLibraryPreview(const QImage& image)
{
    d->setLibraryPreview(image);
}

void Photo::setLibraryPreviewsRGB(const QImage& image)
{
    d->setLibraryPreviewsRGB(image);
}

const QImage& Photo::libraryPreview()
{
    const QImage& img = d->libraryPreview();

    if (img.isNull())
    {
        d->owner()->loadImage(*this);
    }
    return img;
}

const QImage& Photo::libraryPreviewsRGB()
{
    // make sure the library preview is avaialble,
    // if it is not trigger download

    if (d->libraryPreviewsRGB().isNull())
    {
        if (d->libraryPreview().isNull())
            libraryPreview();
        else
            d->owner()->convertImage(*this);
    }

    return d->libraryPreviewsRGB();
}

void Photo::setSrcImagePath(const QString& path)
{
    d->setSrcImagePath(path);
}

const QString& Photo::srcImagePath() const
{
    return d->srcImagePath();
}

void Photo::setExifInfo(const ExifInfo& exifInfo)
{
    d->setExifInfo(exifInfo);
}

ExifInfo& Photo::exifInfo()
{
    return d->exifInfo();
}

const ExifInfo& Photo::exifInfo() const
{
    return d->exifInfo();
}

void Photo::setRating(int rating)
{
    d->setRating(rating);
}

int Photo::rating() const
{
    return d->rating();
}

void Photo::setColorLabel(ColorLabel label)
{
    d->setColorLabel(label);
}

Photo::ColorLabel Photo::colorLabel() const
{
    return d->colorLabel();
}

void Photo::setFlag(Flag flag)
{
    d->setFlag(flag);
}

Photo::Flag Photo::flag() const
{
    return d->flag();
}

bool Photo::isNull() const
{
    return d.isNull();
}

void Photo::setIsDownloading(bool value)
{
    d->setIsDownloading(value);
}

bool Photo::isDownloading() const
{
    return d->isDownloading();
}

void Photo::setKeywords(const QStringList &list)
{
    d->setKeywords(list);
}

QStringList Photo::keywords() const
{
    return d->keywords();
}

void Photo::setOwner(PhotoModel* model)
{
    d->setOwner(model);
}
}
