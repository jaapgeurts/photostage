#include <QDebug>
#include <QVariant>
#include <QDir>

#include "engine/colortransform.h"
#include "photodata.h"

namespace PhotoStage
{
PhotoData::PhotoData() :
    mLibraryPreviewsRGB()
{
}

PhotoData::PhotoData(const Photo& info)
{
    *this = info;
}

PhotoData::PhotoData(const QImage& image, const QString& filename,
    long long id) :
    mLibraryPreviewsRGB(),
    mLibraryPreview(image),
    mSrcImagePath(filename),
    mId(id)
{
}

PhotoData::PhotoData(QSqlQuery& q) :
    mLibraryPreviewsRGB()
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

const QImage& PhotoData::libraryPreview() const
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
}
