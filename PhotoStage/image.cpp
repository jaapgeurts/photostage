#include <QFile>
#include <QDebug>

#include "constants.h"
#include "image.h"
#include "utils.h"
#include "io/pngio.h"
#include "io/jpegio.h"

namespace PhotoStage
{
Image::Image()
{
    d = QSharedPointer<ImagePrivate>(new ImagePrivate());
}

Image::Image(int width, int height, uint16_t* data)
{
    d = QSharedPointer<ImagePrivate>(new ImagePrivate(width, height, data));
}

Image::Image(int width, int height, int depth, uint8_t* data, Rotation rotate)
{
    d = QSharedPointer<ImagePrivate>(new ImagePrivate(width, height, depth, data, rotate));
}

Image::Image(const QSize& size)
{
    d = QSharedPointer<ImagePrivate>(new ImagePrivate(size));
}

Image::Image(const QImage& image)
{
    d = QSharedPointer<ImagePrivate>(new ImagePrivate(image));
}

Image::~Image()
{
}

bool Image::saveToFile(const QString& filename) const
{
//    if (filename.endsWith("jpg", Qt::CaseInsensitive))
//        return JpegIO::saveToFile(*this, filename);

    if (filename.endsWith("png", Qt::CaseInsensitive))
        return PngIO::saveToFile(*this, filename);

    return false;
}

QImage Image::toQImage() const
{
    return d->toQImage();
}

QImage Image::toPreviewImage() const
{
    if (d->mWidth > PREVIEW_IMG_WIDTH && d->mHeight > PREVIEW_IMG_HEIGHT)
        return d->toQImage().scaled(PREVIEW_IMG_WIDTH, PREVIEW_IMG_HEIGHT,
                   Qt::KeepAspectRatio, Qt::SmoothTransformation);
    else
        return d->toQImage();
}

int Image::width() const
{
    return d->mWidth;
}

int Image::height() const
{
    return d->mHeight;
}

int Image::depth() const
{
    return IMG_DEPTH;
}

QSize Image::size() const
{
    return QSize(d->mWidth, d->mHeight);
}

uint16_t* Image::data() const
{
    return d->mPixels;
}

uint16_t* Image::scanLine(int l) const
{
    return &(d->mPixels[l * d->mWidth * IMG_DEPTH]);
}

bool Image::isNull() const
{
    return d->mPixels == nullptr;
}
}
