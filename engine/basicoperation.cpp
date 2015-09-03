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

PhotoStage::Image BasicOperation::execute(const PhotoStage::Image& image, float EV)
{
    int width  = image.width();
    int height = image.height();

    qDebug() << "Exposure Comp settings" << EV << "Avg val" <<  32768 * pow(2, EV);

    Param<float> paramEV("Param<float> EV");
    paramEV.set(EV);

    Buffer                  inBuf(UInt(16), width, height, 3, 0, (uint8_t*)image.data(), "SrcImage");
    Halide::Image<uint16_t> input(inBuf);

    // perform operation
    Func srcData;
    srcData(x, y, c) = input(x, y, c);

    uint16_t* outdata = new uint16_t[width * height * 3];
    Func      process = exposure(srcData, paramEV);
    Buffer    outBuf(UInt(160), width, height, 3, 0, (uint8_t*)outdata, "DstImage");
    process.realize(outBuf);
    outBuf.copy_to_host();

    // Image will take ownership of the buffer
    return PhotoStage::Image(width, height, outdata);
}



}
