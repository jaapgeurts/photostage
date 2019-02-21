#ifndef PHOTOSTAGE_BASICOPERATION_H
#define PHOTOSTAGE_BASICOPERATION_H

#include <Halide.h>
#include <QImage>

#include "image.h"

namespace PhotoStage {
class BasicOperation
{
public:
  BasicOperation();

  PhotoStage::Image execute(const PhotoStage::Image& image, float EV);

private:
  Halide::Var          x, y, c;
  Halide::Func         mProcess;
  Halide::ImageParam   mInput;
  Halide::Param<float> mEV;

  Halide::Func contrast(Halide::Func in);
  Halide::Func brightness(Halide::Func in);
  Halide::Func exposure(Halide::Func in, Halide::Param<float> ev);
  Halide::Func process(Halide::ImageParam input, Halide::Param<float> ev);
};
} // namespace PhotoStage
#endif // PHOTOSTAGE_BASICOPERATION_H
