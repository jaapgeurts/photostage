#include "exivfacade.h"

#include "exiv2lib.h"

namespace PhotoStage {
ExivFacade* ExivFacade::createExivReader()
{
  return new Exiv2Lib();
}

ExivFacade::ExivFacade()
{
}

QString ExifInfo::formatExposure() const
{
  QString s;

  if (exposureTime != nullptr)
    s += "⅟ " + QString::number((int)(1.0f / *exposureTime)) + "s";

  if (aperture != nullptr)
    s += " at ƒ / " + QString::number(*aperture);

  return s;
}

QString ExifInfo::formatDimension() const
{
  return QString::number(width) + "x" + QString::number(height);
}
} // namespace PhotoStage
