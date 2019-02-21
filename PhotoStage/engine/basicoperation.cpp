#include <QDebug>

#include "basicoperation.h"

using namespace Halide;

namespace PhotoStage {
BasicOperation::BasicOperation()
    : x("x"), y("y"), c("c"), mProcess("BasicOperationProcess"),
      mInput(UInt(16), 3, "InputImage"), mEV("EV")
{
  qDebug() << "Compiling halide function";
  mProcess = process(mInput, mEV);
}

Func BasicOperation::contrast(Func in)
{
}

Func BasicOperation::brightness(Func in)
{
}

Func BasicOperation::exposure(Func in, Halide::Param<float> ev)
{
  Func compensated("Exposure Compensation");

  compensated(x, y, c) =
      Halide::cast<uint16_t>(clamp(in(x, y, c) * pow(2, ev), 0, 65535));

  return compensated;
}

Func BasicOperation::process(ImageParam input, Halide::Param<float> ev)
{
  // perform operation
  Func srcData;

  srcData(x, y, c) = input(x, y, c);

  Func process = exposure(srcData, ev);

  //    Var  x_outer, y_outer, x_inner, tile_index;
  //    process.tile(x, y, x_outer, y_outer, x_inner, y_inner, 16, 16)
  //    .fuse(x_outer, y_outer, tile_index)
  //    .parallel(tile_index);

  process.vectorize(x, 8);

  return process;
}

PhotoStage::Image BasicOperation::execute(const PhotoStage::Image& image,
                                          float                    EV)
{
  int width  = image.width();
  int height = image.height();

  //    qDebug() << "Exposure Comp settings" << EV << "Avg val" <<  32768 *
  //    pow(2, EV);

  // construct the input and output buffers.
  // Buffer<UInt(16)>    inBuf(UInt(16), width, height, 3, 0,
  // (uint8_t*)image.data(), "SrcImage");
  Buffer<uint16_t> inBuf(image.data(), width, height, 3);

  uint16_t* outdata = new uint16_t[width * height * 3];
  //    Buffer<UInt(16)>    outBuf(UInt(16), width, height, 3, 0,
  //    (uint8_t*)outdata, "DstImage");
  Buffer<uint16_t> outBuf(outdata, width, height, 3);

  mInput.set(inBuf);
  mEV.set(EV);

  mProcess.realize(outBuf);
  // outBuf.copy_to_host();

  // Image will take ownership of the buffer
  return PhotoStage::Image(width, height, outdata);
}
} // namespace PhotoStage
