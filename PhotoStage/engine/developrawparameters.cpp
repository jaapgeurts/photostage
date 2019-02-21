#include "developrawparameters.h"

namespace PhotoStage {
DevelopRawParameters::DevelopRawParameters()
{
}

DevelopRawParameters::DevelopRawParameters(const ExifInfo& ex_info,
                                           long long       id)
    : id(id), redMultiplier(ex_info.rgbCoeffients[0]),
      greenMultiplier(ex_info.rgbCoeffients[1]),
      blueMultiplier(ex_info.rgbCoeffients[2]),
      rotation((DevelopRawParameters::Rotation)ex_info.rotation)
{
}

DevelopRawParameters::DevelopRawParameters(long long id, float red, float green,
                                           float blue)
    : id(id), redMultiplier(red), greenMultiplier(green), blueMultiplier(blue)
{
}
} // namespace PhotoStage
