#ifndef PHOTOSTAGE_ENGINE_UTILS_H
#define PHOTOSTAGE_ENGINE_UTILS_H

#include <Halide.h>
#include <QImage>

namespace PhotoStage {
class EngineUtils
{
public:
  EngineUtils();

  static EngineUtils* instance();

  void histogram();

  QImage toQImage(int width, int height, uint16_t* data);

private:
  static EngineUtils* mEngineUtils;

  Halide::Var x;
  Halide::Var y;
  Halide::Var c;

  Halide::Func       mToQImage;
  Halide::ImageParam mInput;

  Halide::Func process(Halide::ImageParam input);
  Halide::Func to32Bit(Halide::Func in);
  Halide::Func tosRGB(Halide::Func in);
};
} // namespace PhotoStage
#endif // PHOTOSTAGE_ENGINE_UTILS_H
