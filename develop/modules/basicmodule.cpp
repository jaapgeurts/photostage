#include <QDebug>

#include <math.h>

#include "basicmodule.h"
#include "ui_BasicModule.h"

namespace PhotoStage
{
BasicModule::BasicModule(QWidget* parent) :
    DevelopModule(parent),
    ui(new Ui::BasicModule)
{
    ui->setupUi(this);
}

BasicModule::~BasicModule()
{
    delete ui;
}

//typedef struct
//{
//    double r;       // percent
//    double g;       // percent
//    double b;       // percent
//} rgb;

//typedef struct
//{
//    double h;       // angle in degrees
//    double s;       // percent
//    double v;       // percent
//} hsv;

//static hsv   rgb2hsv(rgb in);
//static rgb   hsv2rgb(hsv in);

//hsv rgb2hsv(rgb in)
//{
//    hsv    out;
//    double min, max, delta;

//    min = in.r < in.g ? in.r : in.g;
//    min = min  < in.b ? min  : in.b;

//    max = in.r > in.g ? in.r : in.g;
//    max = max  > in.b ? max  : in.b;

//    out.v = max;                                // v
//    delta = max - min;

//    if (max > 0.0)     // NOTE: if Max is == 0, this divide would cause a crash
//    {
//        out.s = (delta / max);                  // s
//    }
//    else
//    {
//        // if max is 0, then r = g = b = 0
//        // s = 0, v is undefined
//        out.s = 0.0;
//        out.h = std::numeric_limits<double>::quiet_NaN();                            // its now undefined
//        return out;
//    }

//    if (in.r >= max)                            // > is bogus, just keeps compilor happy
//        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
//    else if (in.g >= max)
//        out.h = 2.0 + ( in.b - in.r ) / delta;     // between cyan & yellow
//    else
//        out.h = 4.0 + ( in.r - in.g ) / delta;     // between magenta & cyan

//    out.h *= 60.0;                              // degrees

//    if (out.h < 0.0)
//        out.h += 360.0;

//    return out;
//}

//rgb hsv2rgb(hsv in)
//{
//    double hh, p, q, t, ff;
//    long   i;
//    rgb    out;

//    if (in.s <= 0.0)         // < is bogus, just shuts up warnings
//    {
//        out.r = in.v;
//        out.g = in.v;
//        out.b = in.v;
//        return out;
//    }
//    hh = in.h;

//    if (hh >= 360.0)
//        hh = 0.0;
//    hh /= 60.0;
//    i   = (long)hh;
//    ff  = hh - i;
//    p   = in.v * (1.0 - in.s);
//    q   = in.v * (1.0 - (in.s * ff));
//    t   = in.v * (1.0 - (in.s * (1.0 - ff)));

//    switch (i)
//    {
//        case 0:
//            out.r = in.v;
//            out.g = t;
//            out.b = p;
//            break;

//        case 1:
//            out.r = q;
//            out.g = in.v;
//            out.b = p;
//            break;

//        case 2:
//            out.r = p;
//            out.g = in.v;
//            out.b = t;
//            break;

//        case 3:
//            out.r = p;
//            out.g = q;
//            out.b = in.v;
//            break;

//        case 4:
//            out.r = t;
//            out.g = p;
//            out.b = in.v;
//            break;

//        case 5:
//        default:
//            out.r = in.v;
//            out.g = p;
//            out.b = q;
//            break;
//    }
//    return out;
//}

//inline float clamp(float l, float h, float v)
//{
//    if (v < l)
//        v = l;

//    if (v > h)
//        v = h;

//    return v;
//}

void BasicModule::onExposureChanged(int value)
{
    qDebug() << "exposure value" << value;

    //    Image img = mPhoto->original().clone();

    //    float mult = /*1.0 + */ value / 100.0;

    //    qDebug() << "Multiplier" << mult;

    //    int width  = img.width();
    //    int height = img.height();

    //    for (int y = 0; y < height; y++)
    //    {
    //        float* line = img.scanLine(y);

    //        for (int x = 0; x < width; x++)
    //        {
    //            rgb from;
    //            hsv out;
    //            from.r = line[x * 3 + 2];
    //            from.g = line[x * 3 + 1];
    //            from.b = line[x * 3];

    //            out = rgb2hsv(from);

    //            out.v *= pow(2, mult);

    //            // TODO: consider what to do with clipping here.
    //            from            = hsv2rgb(out);
    //            line[x * 3]     = clamp(0, 1, from.b);
    //            line[x * 3 + 1] = clamp(0, 1, from.g);
    //            line[x * 3 + 2] = clamp(0, 1, from.r);

    //            //            line[x]     = (uint8_t)((float)line[x] * mult);
    //            //            line[x + 1] = (uint8_t)((float)line[x + 1] * mult);
    //            //            line[x + 2] = (uint8_t)((float)line[x + 2] * mult);
    //        }
    //    }
    //    mPhoto->setLibraryPreview(img);
    //    emit parametersAdjusted();
}
}
