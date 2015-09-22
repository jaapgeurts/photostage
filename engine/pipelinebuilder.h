#ifndef PHOTOSTAGE_PIPELINEBUILDER_H
#define PHOTOSTAGE_PIPELINEBUILDER_H

#include <Halide.h>

#include "image.h"

namespace PhotoStage
{
class PipelineBuilder
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

        PipelineBuilder();
        ~PipelineBuilder();

        void setWhiteBalance(float wbr, float wbg, float wbb);
        void setDomain(int bl, int wp);
        void setColorConversion(float* colorMatrix);
        void setInput(uint16_t* data, int width, int height);
        void setCFAStart(uint32_t dcraw_filter_id);
        void setRotation(int dir);
        void setInterpolationAlgorithm(InterpolationAlgorithm algorithm);

        void prepare();

        PhotoStage::Image execute();

    private:

        // Variables for construction of the pipeline.
        InterpolationAlgorithm mAlgorithm;
        int                    mRotation;
        int                    mWidth;
        int                    mHeight;

        // Halide execution variables
        Halide::Var x;
        Halide::Var y;
        Halide::Var c;

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
