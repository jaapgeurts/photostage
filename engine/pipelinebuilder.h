#ifndef PIPELINE_H
#define PIPELINE_H

#include <QObject>
#include <QImage>

#include <Halide.h>
#include <lcms2.h>

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

            void prepare();

            QImage execute(int width, int height);

        private:

            OutputIntent mIntent;

            // Halide pipeline input parameters
            Halide::ImageParam mInput;

            // White balance params
            Halide::Param<float> mWBRed;
            Halide::Param<float> mWBGreen;
            Halide::Param<float> mWBBlue;

            Halide::Param<int>   mBlacklevel;
            Halide::Param<int>   mWhitePoint;

            Halide::ImageParam   mColorMatrix;

            Halide::Func         mPipeline;

            // Little CMS variable
            cmsHTRANSFORM mHRawTransform;

            void createRawProfileConversion();
    };
}

#endif // PIPELINE_H