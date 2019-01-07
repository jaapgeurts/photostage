#ifndef PHOTOSTAGE_DIMENSIONOPERATION_H
#define PHOTOSTAGE_DIMENSIONOPERATION_H

#include <QImage>
#include <QRect>
#include <Halide.h>

#include "image.h"

namespace  PhotoStage
{
class DimensionOperation
{
    public:

        DimensionOperation();

        PhotoStage::Image rotate(const PhotoStage::Image& image, float angle);
        PhotoStage::Image scale(const PhotoStage::Image& image, const QSize& newDim);
        PhotoStage::Image crop(const PhotoStage::Image& image, const QRect& cropRect);

    private:

};
}
#endif // PHOTOSTAGE_DIMENSIONOPERATION_H
