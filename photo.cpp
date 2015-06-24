#include <QDir>
#include <QDebug>

#include "photo.h"

namespace PhotoStage
{
Photo::Photo() :
    mLibraryPreviewsRGB()
{
}

Photo::Photo(const Photo& info)
{
    *this = info;
}

Photo::Photo(const QImage& image, const QString& filename, long long id) :
    mLibraryPreviewsRGB(),
    mLibraryPreview(image),
    mSrcImagePath(filename),
    id(id)
{
}

Photo::Photo(QSqlQuery& q) :
    mLibraryPreviewsRGB()
{
    id = q.value(0).toInt();
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

Photo::~Photo()
{
}

void Photo::setOriginal(const QImage& image)
{
    mOriginal = image;
}

const QImage& Photo::original() const
{
    return mOriginal;
}

void Photo::setLibraryPreview(const QImage& image)
{
    mLibraryPreview = image;
    // force regeneration of the display image
    mLibraryPreviewsRGB = QImage();
}

const QImage& Photo::libraryPreview()
{
    return mLibraryPreview;
}

const QImage& Photo::libraryPreviewsRGB()
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

void Photo::setSrcImagePath(const QString& path)
{
    mSrcImagePath = path;
}

const QString& Photo::srcImagePath()
{
    return mSrcImagePath;
}

void Photo::setRating(int rating)
{
    if (rating < 0 || rating > 5)
    {
        qDebug() << "Invalid rating:" << rating;
        return;
    }
    mRating = rating;
}

int Photo::rating()
{
    return mRating;
}

void Photo::setColorLabel(Photo::ColorLabel label)
{
    mColorLabel = label;
}

Photo::ColorLabel Photo::colorLabel()
{
    return mColorLabel;
}

void Photo::setFlag(Photo::Flag flag)
{
    mFlag = flag;
}

Photo::Flag Photo::flag()
{
    return mFlag;
}
}
