#ifndef PIPELINE_H
#define PIPELINE_H

#include <QObject>
#include <QImage>

#include <Halide.h>
#include <lcms2.h>

using namespace Halide;

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
        void setInput(Image<uint16_t> input);

        void prepare();

        QImage execute(int width, int height);

    private:

        OutputIntent mIntent;

        // Halide pipeline input parameters
        ImageParam mInput;

        // White balance params
        Param<float> mWBRed;
        Param<float> mWBGreen;
        Param<float> mWBBlue;

        Param<int>   mBlacklevel;
        Param<int>   mWhitePoint;

        ImageParam   mColorMatrix;

        Func         mPipeline;

        // Little CMS variable
        cmsHTRANSFORM mHRawTransform;

        void createRawProfileConversion();
};

#endif // PIPELINE_H
