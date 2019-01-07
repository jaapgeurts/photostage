#ifndef PHOTOSTAGE_STAGE0RAW_H
#define PHOTOSTAGE_STAGE0RAW_H

#include <Halide.h>

#include "image.h"

#include "developrawparameters.h"

namespace PhotoStage
{
class Stage0Raw
{
    public:

        Stage0Raw();
        ~Stage0Raw();

        void setDevelopParams(const QSharedPointer<DevelopRawParameters>& params);

        void setDomain(int bl, int wp);
        void setColorConversion(float* colorMatrix);
        void setInput(uint16_t* data, int width, int height);
        void setCFAStart(uint32_t dcraw_filter_id);

        void prepare();

        PhotoStage::Image execute();

    private:

        // Variables for construction of the pipeline.
        DevelopRawParameters mDevelopParameters;
        int                  mWidth;
        int                  mHeight;
        int                  mRotation;

        // Halide execution variables
        Halide::Var x;
        Halide::Var y;
        Halide::Var c;

        Halide::Var x_outer, y_outer;

        // Halide pipeline input parameters
        Halide::ImageParam mInput;

        // White balance params
        Halide::Param<float>   mWBRed;
        Halide::Param<float>   mWBGreen;
        Halide::Param<float>   mWBBlue;

        Halide::Param<int>     mBlacklevel;
        Halide::Param<int>     mWhitePoint;
        Halide::Param<uint8_t> mCol;
        Halide::Param<uint8_t> mRow;

        Halide::ImageParam     mColorMatrix;

        Halide::Func           mPipeline;

        Halide::Func bilinear(Halide::Func input);
        Halide::Func normalizeToFloat(Halide::Func input);
        Halide::Func channelMultipliers(Halide::Func input);
        Halide::Func transformToXYZ(Halide::Func input);
        Halide::Func applyContrast(Halide::Func input);
        Halide::Func rotate(Halide::Func input, Halide::Expr angle, Halide::Expr centerX, Halide::Expr centerY);
};
}

#endif // PHOTOSTAGE_PIPELINEBUILDER_H
