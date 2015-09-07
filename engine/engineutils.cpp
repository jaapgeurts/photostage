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

Func EngineUtils::process(ImageParam input)
{
    Var  x, y;

    Func convert("convert");

    Expr red   = cast<uint32_t>(input(x, y, 0) >> 8);
    Expr green = cast<uint32_t>(input(x, y, 1) >> 8);
    Expr blue  = cast<uint32_t>(input(x, y, 2) >> 8);

    convert(x, y) = cast<uint32_t>(0xff << 24 | blue << 16 | green << 8 | red  );

    convert.vectorize(x, 8);

    return convert;
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
