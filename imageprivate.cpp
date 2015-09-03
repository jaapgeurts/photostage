#include "image.h"
#include "utils.h"

namespace PhotoStage
{
Image::ImagePrivate::ImagePrivate() :
    mPixels(NULL),
    mWidth(0),
    mHeight(0)
{
}

Image::ImagePrivate::ImagePrivate(int width, int height, uint16_t* data) :
    mWidth(width),
    mHeight(height)
{
    if (data == nullptr)
        mPixels = new uint16_t[width * height * IMG_DEPTH];
    else
        mPixels = data;
}

Image::ImagePrivate::ImagePrivate(int width, int height, int depth, uint8_t* data, Rotation rotate) :
    mWidth(width),
    mHeight(height)

{
    if (data == nullptr)
        mPixels = new uint16_t[width * height * IMG_DEPTH];
    else
        copy8bitBufferToPlanar(data, depth, rotate);
}

Image::ImagePrivate::ImagePrivate(const QSize& size) :
    mPixels(new uint16_t[size.width() * size.height() * IMG_DEPTH]),
    mWidth(size.width()),
    mHeight(size.height())
{
}

Image::ImagePrivate::ImagePrivate(const QImage& image) :
    mWidth(image.width()),
    mHeight(image.height())
{
    copy8bitBufferToPlanar(image.constBits(), 4, DontRotate);
}

Image::ImagePrivate::~ImagePrivate()
{
    if (mPixels != NULL)
        delete [] mPixels;
}

QImage Image::ImagePrivate::toQImage() const
{
    uint8_t* dst = new uint8_t[mWidth * mHeight * 4];

    for (int y = 0; y < mHeight; y++)
        for (int x = 0; x < mWidth; x++)
            for (int c = 0; c < 3; c++)
                dst[y * mWidth * 4 + x * 4 + c] = (uint8_t)(mPixels[c * mWidth * mHeight + y * mWidth + x] / 256.0f);


    // squeeze the image back into an 8bit QImage
    return QImage(dst, mWidth, mHeight, QImage::Format_RGB32, (QImageCleanupFunction)deleteArray<uint8_t>, dst);
}

void Image::ImagePrivate::copy8bitBufferToPlanar(const uint8_t* src, int depth, Rotation rotate)
{
    mPixels = new uint16_t[mWidth * mHeight * IMG_DEPTH];

    if (rotate == DontRotate)
    {
        for (int y = 0; y < mHeight; y++)
            for (int x = 0; x < mWidth; x++)
                for (int c = 0; c < 3; c++)
                    mPixels[c * mWidth * mHeight + y * mWidth + x] =
                        src[mWidth * y * depth + x * depth + c % depth] * 256;

    }
    else if (rotate == Rotate90CCW)
    {
        /* int t = mWidth;
           mWidth  = mHeight;
           mHeight = t;
         */
        for (int y = 0; y < mHeight; y++)
            for (int x = 0; x < mWidth; x++)
                for (int c = 0; c < 3; c++)
                    mPixels[c * mWidth * mHeight + y * mWidth + x] =
                        src[mWidth * y * 4 + x * 4 + c] * 256;

    }
    else if (rotate == Rotate90CW)
    {
        /*      int t = mWidth;
              mWidth  = mHeight;
              mHeight = t;
         */
        for (int y = 0; y < mHeight; y++)
            for (int x = 0; x < mWidth; x++)
                for (int c = 0; c < 3; c++)
                    mPixels[c * mWidth * mHeight + y * mWidth + x] =
                        src[mWidth * y * 4 + x * 4 + c] * 256;

    }
}
}
