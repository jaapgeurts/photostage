#include "dimensionoperation.h"

namespace PhotoStage {
DimensionOperation::DimensionOperation()
{
}

Image DimensionOperation::rotate(const Image& image, float angle)
{
  return Image();
}

Image DimensionOperation::scale(const Image& image, const QSize& newDim)
{
  return Image();
}

Image DimensionOperation::crop(const Image& image, const QRect& cropRect)
{
  return Image();
}

} // namespace PhotoStage
