#ifndef PHOTOSTAGE_DIMENSIONOPERATION_H
#define PHOTOSTAGE_DIMENSIONOPERATION_H

#include <Halide.h>
#include <QImage>
#include <QRect>

#include "image.h"

namespace PhotoStage {
class DimensionOperation
{
public:
  DimensionOperation();

  PhotoStage::Image rotate(const PhotoStage::Image& image, float angle);
  PhotoStage::Image scale(const PhotoStage::Image& image, const QSize& newDim);
  PhotoStage::Image crop(const PhotoStage::Image& image, const QRect& cropRect);

private:
};
} // namespace PhotoStage
#endif // PHOTOSTAGE_DIMENSIONOPERATION_H
