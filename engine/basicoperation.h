#ifndef PHOTOSTAGE_BASICOPERATION_H
#define PHOTOSTAGE_BASICOPERATION_H

#include <Halide.h>
#include <QImage>

namespace PhotoStage
{
class BasicOperation
{
    public:

        BasicOperation();

        QImage execute(const QImage& image, float EV);

    private:

        Halide::Var x, y, c;

        Halide::Image<uint16_t> createImage(const QImage& image);

        Halide::Func contrast(Halide::Func in);
        Halide::Func brightness(Halide::Func in);
        Halide::Func exposure(Halide::Func in, Halide::Param<float> EV);
};
}
#endif // PHOTOSTAGE_BASICOPERATION_H
