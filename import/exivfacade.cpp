#include "exivfacade.h"

#include "exiv2lib.h"

namespace PhotoStage
{
ExivFacade* ExivFacade::createExivReader()
{
    return new Exiv2Lib();
}

ExivFacade::ExivFacade()
{
}

QString ExifInfo::formatExposure() const
{
    return "⅟ " + QString::number((int)(1.0f / exposureTime)) + "s at ƒ / " +
           QString::number(aperture);
}

QString ExifInfo::formatDimension() const
{
    return QString::number(width) + "x" + QString::number(height);
}
}
