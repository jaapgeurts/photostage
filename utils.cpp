#include "utils.h"

namespace PhotoStage
{
    /* fits the source frame into the destination frame and
     * centers the image */
    QRect fitFrame(const QSize& src, const QSize& destFrame)
    {
        int   ws = src.width(); // width source;
        int   hs = src.height();

        int   wd = destFrame.width(); // width destination
        int   hd = destFrame.height();

        float ratio = (float)ws / (float)hs;

        int   x  = 0;
        int   y  = 0;
        int   hn = hd; // new height
        int   wn = wd; // new width

        if (ratio > 1.0f)
        {
            hn = hd / ratio;
            y  = (hd - hn) / 2;
        }
        else
        {
            wn = wd * ratio;
            x  = (wd - wn) / 2;
        }
        return QRect(x, y, wn, hn);
    }
}

// this code is needed for RawSpeed

int rawspeed_get_number_of_processor_cores()
{
    return 1;
}