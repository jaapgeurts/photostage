#include <QDebug>

#include <math.h>

#include "pipelinebuilder.h"

using namespace Halide;

namespace PhotoStage
{
PipelineBuilder::PipelineBuilder() :
    mAlgorithm(Bilinear),
    mRotation(0),
    mWidth(0),
    mHeight(0),
    x("x"),
    y("y"),
    c("c"),
    mInput(type_of<uint16_t>(), 2),
    mColorMatrix(type_of<float>(), 2),
    mPipeline("Stage1Raw")
{
    prepare();
}

PipelineBuilder::~PipelineBuilder()
{
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

    Halide::Image<float> im(3, 3);

    for (int y = 0; y < 3; y++)
        for (int x = 0; x < 3; x++)
            im(x, y) = colorMatrix[y * 3 + x];


    mColorMatrix.set(im);
}

void PipelineBuilder::setInput(uint16_t* data, int width, int height)
{
    Buffer                  buf(UInt(16), width, height, 1, 0, (uint8_t*)data, "Input Image");
    Halide::Image<uint16_t> input(buf);

    // Take of the outer edges to make interpolation easier
    mWidth  = width - 2;
    mHeight = height - 2;

    mInput.set(input);
}

void PipelineBuilder::setCFAStart(uint32_t dcraw_filter_id)
{
    // enter the start position or R
    if (dcraw_filter_id == 0x16161616)
    {
        // BGGR
        mCol.set(1);
        mRow.set(1);
    }
    else if (dcraw_filter_id == 0x61616161)
    {
        // GRBG
        mCol.set(1);
        mRow.set(0);
    }
    else if (dcraw_filter_id == 0x49494949)
    {
        // GBRG
        mCol.set(0);
        mRow.set(1);
    }
    else if (dcraw_filter_id == 0x94949494)
    {
        // RGGB
        mCol.set(0);
        mRow.set(0);
    }
}

void PipelineBuilder::setRotation(int dir)
{
    mRotation = dir;
}

void PipelineBuilder::setInterpolationAlgorithm(PipelineBuilder::InterpolationAlgorithm algorithm)
{
    mAlgorithm = algorithm;
}

//bicubic interpolation
Expr getValue(Expr p0, Expr p1, Expr p2, Expr p3, Expr coo)
{
    return p1 + 0.5f * coo * (p2 - p0 + coo *
           (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3 + coo *
           (3.0f * (p1 - p2) + p3 - p0)));
}

Expr bicubic(Func f, Expr x, Expr y)
{
    Expr one = x - cast<int>(x);
    Expr two = y - cast<int>(y);
    Expr a1  =
        getValue(f(cast<int>(x) - 1, cast<int>(y) - 1),
            f(cast<int>(x), cast<int>(y) - 1),
            f(cast<int>(x) + 1, cast<int>(y) - 1),
            f(cast<int>(x) + 2, cast<int>(y) - 1), one);
    Expr a2 =
        getValue(f(cast<int>(x) - 1, cast<int>(y)),
            f(cast<int>(x), cast<int>(y)),
            f(cast<int>(x) + 1, cast<int>(y)),
            f(cast<int>(x) + 2, cast<int>(y)), one);
    Expr a3 =
        getValue(f(cast<int>(x) - 1, cast<int>(y) + 1),
            f(cast<int>(x), cast<int>(y) + 1),
            f(cast<int>(x) + 1, cast<int>(y) + 1),
            f(cast<int>(x) + 2, cast<int>(y) + 1), one);
    Expr a4 =
        getValue(f(cast<int>(x) - 1, cast<int>(y) + 2),
            f(cast<int>(x), cast<int>(y) + 2),
            f(cast<int>(x) + 1, cast<int>(y) + 2),
            f(cast<int>(x) + 2, cast<int>(y) + 2), one);

    return getValue(a1, a2, a3, a4, two);
}

Func PipelineBuilder::bilinear(Func input)
{
    Func green("green");

    green(x, y) = select((x % 2) + mCol == (y % 2) + mRow,
            (input(x - 1, y) + input(x + 1, y) + input(x, y - 1) + input(x, y + 1)) / 4, // green at blue or red
            input(x, y));

    Func blue("blue");
    blue(x, y) = select((x % 2) == mCol && (y % 2) == mRow,
            input(x, y), // blue at blue
            select((x % 2) == 1 - mCol && (y % 2) == 1 - mRow,
            (input(x - 1, y - 1) + input(x + 1, y - 1) + input(x - 1, y + 1) + input(x + 1, y + 1)) / 4,                                                   // blue at red
            select((y % 2) == 1 - mRow,              // green at blue row
            (input(x, y - 1) + input(x, y + 1)) / 2,
            (input(x - 1, y) + input(x + 1, y)) / 2 )));

    Func red("red");
    red(x, y) = select((x % 2) == 1 - mCol && (y % 2) == 1 - mRow,
            input(x, y), // red at red
            select((x % 2 ) == mCol && (y % 2) == mRow,
            (input(x - 1, y - 1) + input(x + 1, y - 1) + input(x - 1, y + 1) + input(x + 1, y + 1)) / 4,                                                   // red at blue
            select((y % 2) == mRow,              // green at blue row
            (input(x, y - 1) + input(x, y + 1)) / 2,
            (input(x - 1, y) + input(x + 1, y)) / 2 )));

    Func bilinear("bilinear");

    bilinear(x, y, c) =
        select(c == 0, blue(x, y),
            select(c == 2, red(x, y),
            green(x, y)));

    return bilinear;
}

Func PipelineBuilder::normalizeToFloat(Func input)
{
    Func asFloat("asFloat");

    asFloat(x, y, c) = Halide::clamp((input(x, y, c) - mBlacklevel) /
            cast<float>(mWhitePoint - mBlacklevel), 0.0f, 1.0f);

    return asFloat;
}

Func PipelineBuilder::channelMultipliers(Func input)
{
    Func multipliers("ChannelMultipliers");

    multipliers(x, y, c) =
        select(c == 0,  input(x, y, c) * mWBBlue,    // blue
            select(c == 2, input(x, y, c) * mWBRed,     // red
            input(x, y, c) * mWBGreen));

    return multipliers;
}

Func PipelineBuilder::transformToXYZ(Func input)
{
    Func toXYZmatrix("toXYZmatrix");

    RDom k(0, 3);

    // matrix multiply
    toXYZmatrix(x, y, c) = sum(input(x, y, 2 - k) * mColorMatrix(k, c));

    return toXYZmatrix;
}

Expr upperHalf(Expr x, Expr E)
{
    return 1.0f - 0.5f * pow(2.0f - 2.0f * x, E);
}

Expr lowerHalf(Expr x, Expr E)
{
    return 0.5f * pow(2.0f * x, E);
}

Func PipelineBuilder::applyContrast(Func input)
{
    Expr v("v");

    v = input(x, y, c) * 3.0f;

    Expr C("C");
    C = 1.15f;
    Func contrast("contrast");
    contrast(x, y, c) = select(v < 0.5f, lowerHalf(v, C), upperHalf(v, C));
    // contrast(x, y, c) = toXYZmatrix(x, y, c); //select(v < 0.5f, v,v);

    //(uint8_t)(pow(rawimage[pix]/255.0,1.0/gab)*255);

    return contrast;
}

/**
 * @brief Rotates the picture using the specified angle. Currently only supports 90 degrees.
 * @param input
 * @param angle
 * @param centerX
 * @param centerY
 * @return
 */
Func PipelineBuilder::rotate(Func input, Expr angle, Expr centerX, Expr centerY)
{
    Func out;

    out(x, y) = input(y, x);
    return out;
}

void PipelineBuilder::prepare()
{
    // shift the input by 1 so we don't have to do any border checking
    Func shifted("shifted");

    shifted(x, y) = mInput(x + 1, y + 1);

    Func demosaiced("demosaic");
    demosaiced = bilinear(shifted);

    // first convert to float
    Func normalized = normalizeToFloat(demosaiced);
    Func multiplied = channelMultipliers(normalized);
    Func toXYZcolor = transformToXYZ(multiplied);

    //    float gam = 2.2f;
    //    Func  gamma("gamma");
    //    gamma(x, y, c) = pow(toXYZmatrix(x, y, c), 1.0f / gam);

    Func contrast = applyContrast(toXYZcolor);

    Func final ("final");
    final (x, y, c) = cast<uint16_t>(contrast(x, y, c) * 65535.0f);
    //    final (x, y, c) = cast<uint16_t>(toXYZcolor(x, y, c) * 65535.0f);

    if (mRotation == -1)
    {
        Func mrot("Rotation");
        mrot(x, y, c) = final (mWidth - 1 - y,  x, c);
        mPipeline     = mrot;
    }
    else if (mRotation == 1)
    {
        Func mrot("Rotation");
        mrot(x, y, c) = final (y, mHeight - 1 - x, c);
        mPipeline     = mrot;
    }
    else
    {
        mPipeline = final;
    }

    mPipeline.tile(x, y, x_outer, y_outer, 32, 32)
    .parallel(y);
}

PhotoStage::Image PipelineBuilder::execute()
{
    uint16_t* outdata = new uint16_t[mWidth * mHeight * 3];

    if (mRotation == -1 || mRotation == 1)
    {
        Buffer outBuf(UInt(16), mHeight, mWidth, 3, 0, (uint8_t*)outdata, "DstImage");
        mPipeline.realize(outBuf);
        return PhotoStage::Image(mHeight, mWidth, outdata);
    }
    else
    {
        Buffer outBuf(UInt(16), mWidth, mHeight, 3, 0, (uint8_t*)outdata, "DstImage");
        mPipeline.realize(outBuf);
        return PhotoStage::Image(mWidth, mHeight, outdata);
    }
}
}
