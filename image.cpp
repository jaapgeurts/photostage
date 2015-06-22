#include <png.h>

#include <QFile>
#include <QDebug>

#include <engine/colortransform.h>

#include "image.h"

#define PNG_SIG_SIZE 8

Image::Image() :
    mPixels()
{
    mWidth  = 0;
    mHeight = 0;
}

template <typename T_>
static void do_delete(T_ buf[])
{
    qDebug() << "Delete called on mPixels";
    delete[] buf;
}

Image::Image(int width, int height) :
    mPixels(new float[width * height * IMG_NO_CHANNELS], do_delete<float>) // three channels;
{
    mWidth  = width;
    mHeight = height;
}

Image::Image(const QSize& size) :
    mPixels(new float[size.width() * size.height() * IMG_NO_CHANNELS], do_delete<float>)
{
    mWidth  = size.width();
    mHeight = size.height();
}

Image::~Image()
{
}

Image Image::clone() const
{
    qDebug() << "Cloning image";
    Image image;
    image.mWidth  = this->mWidth;
    image.mHeight = this->mHeight;
    image.mPixels = QSharedPointer<float>(new float[mWidth * mHeight * IMG_NO_CHANNELS], do_delete<float>);

    memcpy(image.mPixels.data(), this->mPixels.data(), sizeof(float) * mWidth * mHeight * IMG_NO_CHANNELS);
    return image;
}

static void userReadData(png_structp pngPtr, png_bytep data, png_size_t length)
{
    png_voidp a = png_get_io_ptr(pngPtr);

    ((QFile*)a)->read((char*)data, length);
}

Image Image::fromFile(const QString& filename)
{
    png_bytep* rowPtrs = NULL;
    uint16_t*  data    = NULL;

    QFile      file(filename);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "can't open image" << filename;
        return Image();
    }

    char header[PNG_SIG_SIZE];

    if (file.read(header, PNG_SIG_SIZE) < 0)
    {
        qDebug() << "PNG: error reading header";
        return Image();
    }

    if (png_sig_cmp((png_const_bytep)header, 0, PNG_SIG_SIZE) != 0)
    {
        qDebug() << "File" << filename << "is not a PNG file";
        return Image();
    }

    png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!pngPtr)
    {
        qDebug() << "PNG: error setting readstruct";
        return Image();
    }

    png_infop infoPtr = png_create_info_struct(pngPtr);

    if (!infoPtr)
    {
        qDebug() << "PNG: error setting info ptr";
        return Image();
    }

    if (setjmp(png_jmpbuf(pngPtr)))
    {
        // error occured here
        png_destroy_read_struct(&pngPtr, &infoPtr, (png_infopp)0);

        if (rowPtrs != NULL)
            delete [] rowPtrs;

        if (data != NULL)
            delete [] data;
        return Image();
    }

    // set our custom read function so we can read from QFile
    png_set_read_fn(pngPtr, (png_voidp) & file, userReadData);

    // tell PNG to not read the header again (as we've already read it)
    png_set_sig_bytes(pngPtr, PNG_SIG_SIZE);

    // read the info block
    png_read_info(pngPtr, infoPtr);

    png_set_swap(pngPtr);

    int         width  = png_get_image_width(pngPtr, infoPtr);
    int         height = png_get_image_height(pngPtr, infoPtr);

    png_uint_32 bitdepth   = png_get_bit_depth(pngPtr, infoPtr);
    png_uint_32 channels   = png_get_channels(pngPtr, infoPtr);
    png_uint_32 color_type = png_get_color_type(pngPtr, infoPtr);

    switch (color_type)
    {
        case PNG_COLOR_TYPE_PALETTE:
            qDebug() << "Unsupported color type: PALETTE";
            return Image();
            break;

        case PNG_COLOR_TYPE_GRAY:
            qDebug() << "Unsupported color type: GRAY";
            return Image();
            break;

        case PNG_COLOR_TYPE_RGB:
            break;
    }

    if (bitdepth != 16)
    {
        qDebug() << "Image not 16 bits";
        return Image();
    }

    if (channels != 3)
    {
        qDebug() << "Channels not equal to 3";
        return Image();
    }

    rowPtrs = new png_bytep[height];
    data    = new uint16_t[width * height * channels];
    const unsigned int stride = width * channels;

    for (size_t i = 0; i < height; i++)
    {
        png_uint_32 q =  stride * i;
        rowPtrs[i] = (png_bytep)data + q * 2;
    }

    png_read_image(pngPtr, rowPtrs);

    delete[] rowPtrs;
    png_destroy_read_struct(&pngPtr, &infoPtr, (png_infopp)0);

    Image image(width, height);

    // convert the image to float rep.

    for (int y = 0; y < height; y++)
    {
        float* pixels = image.scanLine(y);
        for (int x = 0; x < width; x++)
        {
            pixels[x * IMG_NO_CHANNELS]     =  (float)(data[y * stride + x * 3 + 2] / 65535.0);
            pixels[x * IMG_NO_CHANNELS + 1] =  (float)(data[y * stride + x * 3 + 1] / 65535.0);
            pixels[x * IMG_NO_CHANNELS + 2] =  (float)(data[y * stride + x * 3 + 0] / 65535.0);
        }
    }
    delete [] data;

    return image;
}

Image Image::fromQImage(const QImage& image)
{
    // TODO: now only RGB32 images are supported
    if (image.format() != QImage::Format_RGB32)
    {
        qDebug() << "Image is not in RGB32 format. Actual QImage::Format" << image.format();
        return Image();
    }

    //do color conversion here
    ColorTransform toWorking = ColorTransform::getTransform("sRGB", WORKING_COLOR_SPACE, ColorTransform::FORMAT_RGB32, ColorTransform::FORMAT_FLOAT);
    return toWorking.transformFromQImage(image);
}

QImage Image::toQImage() const
{
    ColorTransform toRgb = ColorTransform::getTransform(WORKING_COLOR_SPACE, "sRGB", ColorTransform::FORMAT_FLOAT, ColorTransform::FORMAT_RGB32);

    return toRgb.transformToQImage(*this);
}

static void userWriteData(png_structp pngPtr, png_bytep data, png_size_t length)
{
    png_voidp a = png_get_io_ptr(pngPtr);

    ((QFile*)a)->write((char*)data, length);
}

static void userFlush(png_structp pngPtr)
{
    png_voidp a = png_get_io_ptr(pngPtr);

    ((QFile*)a)->flush();
}

bool Image::saveToFile(const QString& filename) const
{
    // FIXME image is not normalized.
    QFile     file(filename);
    uint16_t* row = NULL;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "Can't open file for writing";
        return false;
    }

    png_structp pngPtr;
    png_infop   infoPtr;

    pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (pngPtr == NULL)
    {
        qDebug() << "PNG:Can't create write struct";
        return false;
    }

    infoPtr = png_create_info_struct(pngPtr);

    if (infoPtr == NULL)
    {
        qDebug() << "PNG:can't create info struct";
        return false;
    }

    if (setjmp(png_jmpbuf(pngPtr)))
    {
        qDebug() << "PNG error during creating";

        if (row != NULL)
            delete [] row;
        return false;
    }

    png_set_write_fn(pngPtr, (png_voidp) & file, userWriteData, userFlush);

    png_set_IHDR(pngPtr, infoPtr, mWidth, mHeight, 16, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_text titleText;
    titleText.compression = PNG_TEXT_COMPRESSION_NONE;
    titleText.key         = "Title";
    titleText.text        = "PhotoStage preview thumb";
    png_set_text(pngPtr, infoPtr, &titleText, 1);
    png_write_info(pngPtr, infoPtr);

    png_set_swap(pngPtr);

    row = new uint16_t[mWidth * IMG_NO_CHANNELS];

    for (int y = 0; y < mHeight; y++)
    {
        float* inLine = scanLine(y);

        for (int x = 0; x < mWidth; x++)
        {
            // FIXME: image is not normalized to 0..1 so values results will be clipped.
            // should use littleCMS to convert to 16 bit correctly
            row[x * 3 + 2] = (uint16_t)(inLine[x * IMG_NO_CHANNELS] * 0xffff);
            row[x * 3 + 1] = (uint16_t)(inLine[x * IMG_NO_CHANNELS + 1] * 0xffff);
            row[x * 3 + 0] = (uint16_t)(inLine[x * IMG_NO_CHANNELS + 2] * 0xffff);
        }
        png_write_row(pngPtr, (png_const_bytep)row);
    }

    png_write_end(pngPtr, infoPtr);

    png_destroy_write_struct(&pngPtr, &infoPtr);

    delete [] row;
}

int Image::width() const
{
    return mWidth;
}

int Image::height() const
{
    return mHeight;
}

QSize Image::size() const
{
    return QSize(mWidth, mHeight);
}

float* Image::data() const
{
    return mPixels.data();
}

float* Image::scanLine(int l) const
{
    return &(mPixels.data()[l * mWidth * IMG_NO_CHANNELS]);
}

bool Image::isNull() const
{
    return mPixels.isNull();
}
