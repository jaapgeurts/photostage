#include <QDebug>

#include "engine/engineutils.h"
#include "image.h"
#include "utils.h"

namespace PhotoStage {
Image::ImagePrivate::ImagePrivate() : mPixels(nullptr), mWidth(0), mHeight(0)
{
}

Image::ImagePrivate::ImagePrivate(int width, int height, uint16_t* data)
    : mWidth(width), mHeight(height)
{
  if (data == nullptr)
    mPixels = new uint16_t[width * height * IMG_DEPTH];
  else
    mPixels = data;
}

Image::ImagePrivate::ImagePrivate(int width, int height, int depth,
                                  uint8_t* data, Rotation rotate)
    : mWidth(width), mHeight(height)

{
  if (data == nullptr)
    mPixels = new uint16_t[width * height * IMG_DEPTH];
  else
    copy8bitBufferToPlanar(data, depth, rotate);
}

Image::ImagePrivate::ImagePrivate(const QSize& size)
    : mPixels(new uint16_t[size.width() * size.height() * IMG_DEPTH]),
      mWidth(size.width()), mHeight(size.height())
{
}

Image::ImagePrivate::ImagePrivate(const QImage& image)
    : mWidth(image.width()), mHeight(image.height())
{
  copy8bitBufferToPlanar(image.constBits(), 4, DontRotate);
}

Image::ImagePrivate::~ImagePrivate()
{
  if (mPixels != NULL)
    delete[] mPixels;
}

QImage Image::ImagePrivate::toQImage() const
{
  //    uint8_t* dst = new uint8_t[mWidth * mHeight * 4];

  //    for (int y = 0; y < mHeight; y++)
  //        for (int x = 0; x < mWidth; x++)
  //            for (int c = 0; c < 3; c++)
  //            {
  //                dst[y * mWidth * 4 + x * 4 + c] = (uint8_t)(mPixels[c *
  //                mWidth * mHeight + y * mWidth + x] >> 8); dst[y * mWidth * 4
  //                + x * 4 + 3] = 0xff;
  //                //                dst[((y * mWidth) << 2) + (x << 2) +c] =
  //                (uint8_t)(mPixels[c * mWidth * mHeight + y * mWidth + x] >>
  //                8);
  //                //                dst[((y * mWidth) << 2) + (x << 2) + 3] =
  //                0xff;
  //            }

  // squeeze the image back into an 8bit QImage
  // return QImage(dst, mWidth, mHeight, QImage::Format_RGB32,
  // (QImageCleanupFunction)deleteArray<uint8_t>, dst);

  return EngineUtils::instance()->toQImage(mWidth, mHeight, mPixels);
}

void Image::ImagePrivate::copy8bitBufferToPlanar(const uint8_t* src, int depth,
                                                 Rotation rotate)
{
  mPixels = new uint16_t[mWidth * mHeight * IMG_DEPTH];

  if (rotate == DontRotate)
  {
    for (int y = 0; y < mHeight; y++)
      for (int x = 0; x < mWidth; x++)
        for (int c = 0; c < 3; c++)
          mPixels[c * mWidth * mHeight + y * mWidth + x] =
              ((uint16_t)src[mWidth * y * depth + x * depth + c % depth]) << 8;
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
              ((uint16_t)src[mWidth * y * depth + x * depth + c % depth]) << 8;
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
              ((uint16_t)src[mWidth * y * depth + x * depth + c % depth]) << 8;
  }
}
} // namespace PhotoStage
