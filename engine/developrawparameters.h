#ifndef PHOTOSTAGE_DEVELOPRAWPARAMETERS
#define PHOTOSTAGE_DEVELOPRAWPARAMETERS

#include <QSharedPointer>

#include "database/developitems.h"
#include "import/exiv2lib.h"

namespace  PhotoStage
{
class DevelopRawParameters : public DevelopParameterItem
{
    public:

        enum InterpolationAlgorithm
        {
            NearestNeighbour = 1,
            Bilinear,
            Bicubic,
            VariableNumberGradients,
            PatternedPixelGrouping,
            AdaptiveHomogeneityDirected
        };

        enum Rotation
        {
            NotRotated       = 1, // No rotation Needed
            FlipY            = 2, // flip over y
            Rotate180        = 3, // rotate 180 (or flip x flip y)
            FlipX            = 4, // flip over x
            Rotate90CWFlipY  = 5, // rotate 90 CW & flip over y
            Rotate90CW       = 6, // rotate 90 CW
            Rotate90CCWFlipY = 7, // rotate 90 CCW flip over y
            Rotate90CCW      = 8  // rotate 90 CCW
        };

        DevelopRawParameters();
        DevelopRawParameters(const ExifInfo& ex_info,long long id = -1);
        DevelopRawParameters(long long id, float red, float green, float blue);

        long long              id;
        float                  redMultiplier;
        float                  greenMultiplier;
        float                  blueMultiplier;

        Rotation               rotation;

        InterpolationAlgorithm algorithm;
};
}
#endif // PHOTOSTAGE_DEVELOPRAWPARAMETERS
