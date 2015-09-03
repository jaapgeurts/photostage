#ifndef PHOTOSTAGE_PIPELINEBUILDER_H
#define PHOTOSTAGE_PIPELINEBUILDER_H

#include <Halide.h>
#include <lcms2.h>

#include "image.h"

namespace PhotoStage
{
class PipelineBuilder
{
    public:

        enum OutputIntent
        {
            LibraryPreview = 1,
            DevelopPreview,
            FinalOutput
        };

        PipelineBuilder();
        ~PipelineBuilder();

        void setIntent(OutputIntent intent);

        void setWhiteBalance(float wbr, float wbg, float wbb);
        void setDomain(int bl, int wp);
        void setColorConversion(float* colorMatrix);
        void setInput(Halide::Image<uint16_t> input);
        void setCFAStart(uint32_t dcraw_filter_id);
        void setRotation(int dir);

        void prepare();

        PhotoStage::Image execute(int width, int height);

    private:

        OutputIntent mIntent;

        // Halide pipeline input parameters
        Halide::ImageParam  mInput;

        Halide::Param<bool> mIsSRaw;
        int                 mRotation;

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

        //        int                    mWidth;
        //        int                    mHeight;

        // Little CMS variable
        cmsHTRANSFORM mHRawTransform;

        void createRawProfileConversion();
};
}

#endif // PHOTOSTAGE_PIPELINEBUILDER_H
