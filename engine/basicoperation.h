#ifndef PHOTOSTAGE_BASICOPERATION_H
#define PHOTOSTAGE_BASICOPERATION_H

#include <QImage>
#include <Halide.h>

#include "image.h"

namespace PhotoStage
{
class BasicOperation
{
    public:

        BasicOperation();

        PhotoStage::Image execute(const PhotoStage::Image& image, float EV);

    private:

        Halide::Var x, y, c;

        Halide::Func contrast(Halide::Func in);
        Halide::Func brightness(Halide::Func in);
        Halide::Func exposure(Halide::Func in, Halide::Param<float> EV);
};
}
#endif // PHOTOSTAGE_BASICOPERATION_H
