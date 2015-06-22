#include <QDebug>

#include "pipelinebuilder.h"

using namespace Halide;

namespace PhotoStage
{
    PipelineBuilder::PipelineBuilder() :
        mInput(type_of<uint16_t>(), 2),
        mColorMatrix(type_of<float>(), 2),
        mHRawTransform(NULL)
    {
        createRawProfileConversion();
    }

    PipelineBuilder::~PipelineBuilder()
    {
        if (mHRawTransform != NULL)
            cmsDeleteTransform(mHRawTransform);
    }

    void PipelineBuilder::createRawProfileConversion()
    {
        mHRawTransform = NULL;

        cmsHPROFILE hInProfile, hOutProfile;

        hInProfile = cmsOpenProfileFromFile(
            "/Users/jaapg/Development/PhotoStage/PhotoStage/ICCProfiles/D65_XYZ.icc",
            "r");
        hOutProfile = cmsOpenProfileFromFile(
            "/Users/jaapg/Development/PhotoStage/PhotoStage/ICCProfiles/MelissaRGB.icc",
            "r");
        //    hInProfile = cmsCreateXYZProfile();
        //    hOutProfile = cmsCreate_sRGBProfile();

        // for using checking wether the input range conforms to XYZ
#define TYPE_XYZN_FLT (FLOAT_SH(1) | COLORSPACE_SH(PT_XYZ) | EXTRA_SH(1) | \
    CHANNELS_SH(3) | BYTES_SH(4))

        mHRawTransform = cmsCreateTransform(hInProfile,
                TYPE_RGB_16_PLANAR,
                hOutProfile,
                TYPE_BGRA_8,
                INTENT_PERCEPTUAL,
                0);

        cmsCloseProfile(hInProfile);
        cmsCloseProfile(hOutProfile);
    }

    void PipelineBuilder::setIntent(PipelineBuilder::OutputIntent intent)
    {
        mIntent = intent;
    }

    void PipelineBuilder::setWhiteBalance(float wbr, float wbg, float wbb)
    {
        mWBRed.set(wbr);
        mWBGreen.set(wbg);
        mWBBlue.set(wbb);
    }

    void PipelineBuilder::setDomain(int bl, int wp)
    {
        mBlacklevel.set(bl);
        mWhitePoint.set(wp);
    }

    void PipelineBuilder::setColorConversion(float* colorMatrix)
    {
        // convert the colorMatrix array to Halide

        Image<float> im(3, 3);

        for (int y = 0; y < 3; y++)
            for (int x = 0; x < 3; x++)
                im(x, y) = colorMatrix[y * 3 + x];


        mColorMatrix.set(im);
    }

    void PipelineBuilder::setInput(Image<uint16_t> input)
    {
        mInput.set(input);
    }

    Expr upperHalf(Expr x, Expr E)
    {
        return 1.0f - 0.5f * pow(2.0f - 2.0f * x, E);
    }

    Expr lowerHalf(Expr x, Expr E)
    {
        return 0.5f * pow(2.0f * x, E);
    }

    void PipelineBuilder::prepare()
    {
        Var  x("x"), y("y"), c("c");

        Func shifted("shifted");

        shifted(x, y) = mInput(x + 1, y + 1);

        Func red("red");
        red(x, y) = select((x % 2) == 1 && (y % 2) == 1, shifted(x, y), // red at red
                select((x % 2 ) == 0 && (y % 2) == 0,  (shifted(x - 1, y - 1) +
                shifted(x + 1, y - 1) +
                shifted(x - 1, y + 1) +
                shifted(x + 1, y + 1)) / 4,                                       // red at blue
                select((y % 2) == 0,          // green at blue row
                (shifted(x, y - 1) + shifted(x, y + 1)) / 2,
                (shifted(x - 1, y) + shifted(x + 1, y)) / 2 )));

        Func blue("blue");
        blue(x, y) = select((x % 2) == 0 && (y % 2) == 0, shifted(x, y), // blue at blue
                select((x % 2) == 1 && (y % 2) == 1,  (shifted(x - 1, y - 1) +
                shifted(x + 1, y - 1) +
                shifted(x - 1, y + 1) +
                shifted(x + 1, y + 1)) / 4,                                       // blue at red
                select((y % 2) == 1,          // green at blue row
                (shifted(x, y - 1) + shifted(x, y + 1)) / 2,
                (shifted(x - 1, y) + shifted(x + 1, y)) / 2 )));

        Func green("green");
        green(x, y) = select((x % 2) == (y % 2), (shifted(x - 1, y) +
                shifted(x + 1, y) +
                shifted(x, y - 1) +
                shifted(x, y + 1)) / 4,                                    // green at blue or red
                shifted(x, y));

        Func bilinear("bilinear");
        bilinear(x, y, c) =
            select(c == 0,  blue(x, y),
                select(c == 2, red(x, y),
                green(x, y)));

        Func demosaic("demosaic");
        demosaic = bilinear;

        // first convert to float
        Func asFloat("asFloat");
        asFloat(x, y, c) =
            Halide::clamp((demosaic(x, y, c) - mBlacklevel) /
                cast<float>(mWhitePoint - mBlacklevel), 0.0f, 1.0f);
        // todo: clip this result

        Func applyWhiteBalance("applyWhiteBalance");
        applyWhiteBalance(x, y, c) =
            select(c == 0,  asFloat(x, y, c) * mWBBlue,// blue
                select(c == 2, asFloat(x, y, c) * mWBRed, // red
                asFloat(x, y, c) * mWBGreen));

        Func toXYZmatrix("toXYZmatrix");
        toXYZmatrix(x, y, c) = mColorMatrix(0, c) * applyWhiteBalance(x, y, 2) +
            mColorMatrix(1, c) * applyWhiteBalance(x, y, 1) +
            mColorMatrix(2, c) * applyWhiteBalance(x, y, 0);

        //    float gam = 1.8f;
        //    Func  gamma("gamma");
        //    gamma(x, y, c) = pow(toXYZmatrix(x, y, c), 1.0f / gam);

        Expr v("v");
        v = toXYZmatrix(x, y, c) * 3.5f;

        Expr C("C");
        C = 1.15f;
        Func contrast("contrast");
        contrast(x, y, c) = select(v < 0.5f, lowerHalf(v, C), upperHalf(v, C));
        // contrast(x, y, c) = toXYZmatrix(x, y, c); //select(v < 0.5f, v,v);

        //(uint8_t)(pow(rawimage[pix]/255.0,1.0/gab)*255);
        Func final ("final");
        final (x, y, c) = cast<uint16_t>(contrast(x, y, c) * 65535.0f);

        Var x_outer, y_outer, x_inner, y_inner, tile_index;
        final.tile(x, y, x_outer, y_outer, x_inner, y_inner, 256, 256)
        .fuse(x_outer, y_outer, tile_index)
        .parallel(tile_index);

        mPipeline = final;
    }

    QImage PipelineBuilder::execute(int width, int height)
    {
        QImage          image = QImage(width, height, QImage::Format_RGB32);

        Image<uint16_t> out     = mPipeline.realize(width, height, 3);
        uint16_t*       outdata = (uint16_t*)out.data();

        cmsDoTransform(mHRawTransform, outdata, image.bits(), width * height);

        // images are stored in Halide as planar
        //        for (int y = 0; y < height; y++)
        //        {
        //            uint8_t* dst = image.scanLine(y);

        //            for (int x = 0; x < width; x++)
        //            {
        //                for (int c = 0; c < 3; c++)
        //                    dst[x * 4 + c] =
        //                        (uint8_t)(outdata[c * width * height + y * width + x] *
        //                        255.0f);
        //            }
        //        }

        return image;
    }
}