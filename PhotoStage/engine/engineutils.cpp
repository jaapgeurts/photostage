#include <QDebug>

#include "engineutils.h"
#include "utils.h"

using namespace Halide;

namespace PhotoStage
{
EngineUtils* EngineUtils::mEngineUtils = nullptr;

EngineUtils* EngineUtils::instance()
{
    if (mEngineUtils == nullptr)
        mEngineUtils = new EngineUtils();
    return mEngineUtils;
}

EngineUtils::EngineUtils() :
    x("x"),
    y("y"),
    c("c"),
    mInput(UInt(16), 3, "InputImage")
{
    qDebug() << "Setting up engine utils";
    mToQImage = process(mInput);
}

void EngineUtils::histogram()
{
    /* ImageParam input(UInt(8), 2);
       Func histogram;
       Var x;
       RDom r(input); // Iterate over all pixels in the input
       histogram(x) = 0;
       histogram(input(r.x, r.y)) = histogram(input(r.x, r.y)) + 1;
     */
}

Func EngineUtils::to32Bit(Func in)
{
    Func convert("convert");

    Expr red   = cast<uint32_t>(in(x, y, 0) >> 8);
    Expr green = cast<uint32_t>(in(x, y, 1) >> 8);
    Expr blue  = cast<uint32_t>(in(x, y, 2) >> 8);

    convert(x, y) = cast<uint32_t>(0xff << 24 | blue << 16 | green << 8 | red  );

    convert.vectorize(x, 8);

    return convert;
}

Func EngineUtils::tosRGB(Func in)
{
    Func transform("transform");

    // XYZ -> ProPhoto
    //    float rawmatrix[][3] = {
    //        {  2.818, -0.536, -0.106 },
    //        { -1.151,  1.627,  0.043},
    //        {  0.000,  0.000,  1.212}
    //    };

    // sRGB -> ProPhoto
    //    float rawmatrix[][3] = {
    //        {0.410, 0.135, 0.031},
    //        {0.290, 0.710, 0.000},
    //        {0.000, 0.000, 0.825}
    //    };

    //ProPhoto->sRGB
    float rawmatrix[][3] = {
        { 2.142, -0.656, -0.310 },
        {  -0.233,  1.204,  0.004},
        {  -0.014, -0.138,  0.874}
    };

    /*float sum;

       for (int i = 0; i < 3; i++)
       {
        sum = 0;

        for (int j = 0; j < 3; j++)
            sum += rawmatrix[i][j];

        for (int j = 0; j < 3; j++)
            rawmatrix[i][j] /= sum;
       }*/

    Halide::Image<float> matrix(3, 3);

    for (int y = 0; y < 3; y++)
        for (int x = 0; x < 3; x++)
            matrix(x, y) = rawmatrix[x][y];


    transform(x, y, c) = clamp(cast<uint16_t>(in(x, y, 0 ) * matrix(0, c) +
            in(x, y, 1 ) * matrix(1, c) +
            in(x, y, 2 ) * matrix(2, c)), 0, 65535);

    return transform;
}

Func EngineUtils::process(ImageParam input)
{
    Func src;

    src(x, y, c) = input(x, y, c);
    return to32Bit(src);
}

QImage EngineUtils::toQImage(int width, int height, const uint16_t* data)
{
    Buffer   inBuf(UInt(16), width, height, 3, 0, (uint8_t*)data, "SrcImage");

    uint8_t* outdata = new uint8_t[width * height * 4];
    Buffer   outBuf(UInt(32), width, height, 0, 0, (uint8_t*)outdata, "DstImage");

    mInput.set(inBuf);
    mToQImage.realize(outBuf);

    return QImage(outdata, width, height, 4 * width, QImage::Format_RGB32,
               (QImageCleanupFunction)deleteArray<uint8_t>, outdata);
}
}
