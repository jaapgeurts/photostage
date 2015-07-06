#include <QDebug>

#include "photo.h"
#include "photodata.h"

namespace PhotoStage
{
Photo::Photo()
{

}

Photo::Photo(const Photo& info)
{
    *this = info;
}

Photo::Photo(const QImage& image, const QString& filename, long long id)
{
    d = QSharedPointer<PhotoData>(new PhotoData(image, filename, id));
}

Photo::Photo(QSqlQuery& q)
{
    d = QSharedPointer<PhotoData>(new PhotoData(q));
}

long long Photo::id() const
{
    return d->id();
}

void Photo::setOriginal(const QImage &image)
{
    d->setOriginal(image);
}

const QImage &Photo::original() const
{
    return d->original();
}

void Photo::setLibraryPreview(const QImage &image)
{
    d->setLibraryPreview(image);
}

const QImage &Photo::libraryPreview() const
{
    return d->libraryPreview();
}

const QImage &Photo::libraryPreviewsRGB() const
{
    return d->libraryPreviewsRGB();
}

void Photo::setSrcImagePath(const QString &path)
{
    d->setSrcImagePath(path);
}

const QString &Photo::srcImagePath() const
{
    return d->srcImagePath();
}

void Photo::setExifInfo(const ExifInfo &exifInfo)
{
    d->setExifInfo(exifInfo);
}

ExifInfo &Photo::exifInfo()
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

}
