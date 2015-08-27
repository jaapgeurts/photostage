#include <QDebug>

#include "basicoperation.h"

using namespace Halide;

namespace PhotoStage
{
BasicOperation::BasicOperation() :
    x("x"),
    y("y"),
    c("c")
{
}

Func BasicOperation::contrast(Func in)
{
}

Func BasicOperation::brightness(Func in)
{
}

Func BasicOperation::exposure(Func in, Halide::Param<float> EV)
{
    Func compensated("Exposure Compensation");

    compensated(x, y, c) = Halide::cast<uint16_t>(clamp(in(x, y, c) * pow(2, EV), 0, 65535));
    return compensated;
}

QImage BasicOperation::execute(const QImage& image, float EV)
{
    int width  = image.width();
    int height = image.height();

    qDebug() << "Exposure Comp settings" << EV << "Avg val" <<  32768 * pow(2, EV);

    Param<float> paramEV("Param<float> EV");
    paramEV.set(EV);

    Image<uint16_t> input = createImage(image);

    // perform operation
    Func srcData;
    srcData(x, y, c) = input(x, y, c);

    Func            process = exposure(srcData, paramEV);
    Image<uint16_t> output  = process.realize(image.width(), image.height(), 3);

    uint16_t*       outdata = (uint16_t*)output.data();

    // squeeze the image back into an 8bit QImage
    // images are stored in Halide as planar
    QImage outImage(width, height, QImage::Format_RGB32);

    for (int y = 0; y < height; y++)
    {
        uint8_t* dst = outImage.scanLine(y);

        for (int x = 0; x < width; x++)
            for (int c = 0; c < 3; c++)
                dst[x * 4 + c] = (uint8_t)(outdata[c * width * height + y * width + x] / 256.0f);

    }

    return outImage;
}

Image<uint16_t> BasicOperation::createImage(const QImage& image)
{
    int                     width  = image.width();
    int                     height = image.height();

    Halide::Image<uint16_t> img(width, height, 3, "Source Image");
    uint16_t*               dst = img.data();

    // const uint8_t*          src = image.constBits();

    // copy the image to Halide Image as planar
    for (int y = 0; y < height; y++)
    {
        const uint8_t* src = image.constScanLine(y);

        for (int x = 0; x < width; x++)
            for (int c = 0; c < 3; c++)
                dst[c * width * height + y * width + x] = src[x * 4 + c] * 256;

    }

    return img;
}
}
