#include <QPainter>
#include <QPen>
#include <QColor>
#include <QDebug>

#include "histogram.h"

namespace PhotoStage
{
Histogram::Histogram(QWidget* parent) : QWidget(parent)
{
    setMinimumHeight(120);
    setMaximumHeight(120);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    reset();
}

//void Histogram::setImageData(const PhotoData &image)
void Histogram::setImageData(const QImage& image)
{
    mImage = image;
    recalculate();
}

void Histogram::reset()
{
    memset(mChannelRed, 0, sizeof(uint32_t) * BIN_SIZE);
    memset(mChannelBlue, 0, sizeof(uint32_t) * BIN_SIZE);
    memset(mChannelGreen, 0, sizeof(uint32_t) * BIN_SIZE);

    mMaxAll = 0;
}

void Histogram::clear()
{
    reset();
    update();
}

void Histogram::recalculate()
{
    // the default BIN_SIZE = 256
    //set all values to 0
    reset();

    int pos = 0;

    // first do uniform distribution (linear transform)
    // TODO: do we need to count all or just 1/2 or 1/4 of the pic (skip 2 or 4)
    float factor = 1.0f;

    int   count = mImage.width();

    for (int j = 0; j < mImage.height(); j++)
    {
        const uint8_t* pixels = mImage.constScanLine(j);

        for (int i = 0; i < count; i++)
        {
            int red   = pixels[i * 4 + 2];
            int green = pixels[i * 4 + 1];
            int blue  = pixels[i * 4 + 0];

            mChannelRed[(int)(red * factor)]++;
            mChannelGreen[(int)(green * factor)]++;
            mChannelBlue[(int)(blue * factor)]++;

            // use green as the average luminance.
            // use this value to scale the y axis of the histogram when painting

            pos = (int)(green * factor);

            // skip blown out or fully black areas
            if (pos > 5 && pos < 250 && mMaxAll < mChannelGreen[pos])
                mMaxAll = mChannelGreen[pos];
        }
    }
    mMaxAll = (float)mMaxAll * 1.05f;
    //    qDebug() << "Counts of red[0]" << mChannelRed[0];
    //    qDebug() << "Counts of green[0]" << mChannelGreen[0];
    //    qDebug() << "Counts of blue[0]" << mChannelBlue[0];
    //    qDebug() << "Maxall" << mMaxAll << "pos" << pos;
    update();
}

uint32_t getHistValue(uint32_t* buf, int pos, int fromsize)
{
    // weighted interpolation between two values
    float loc    = (float)pos / (float)fromsize * (float)(BIN_SIZE - 1);
    float frac   = loc - (int)loc;
    int   newpos = (int)loc;

    return buf[newpos] * (1.0f - frac) + buf[newpos + 1] * frac;
}

void Histogram::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    int      ww       = width();     // widget width
    int      wh       = height();     // widget height
    QPen     penRed   = QPen(QColor(Qt::red).lighter(120));
    QPen     penGreen = QPen(QColor(Qt::green).lighter(120));
    QPen     penBlue  = QPen(QColor(Qt::blue).lighter(120));

    painter.setCompositionMode(QPainter::CompositionMode_Plus);

    if (mMaxAll == 0)
        return;

    for (int x = 0; x < ww; x++)
    {
        painter.setPen(penRed);
        painter.drawLine(x, wh - 1, x, wh - 1 -
            (int)((float)getHistValue(mChannelRed,
            x, ww) / (float)mMaxAll * (float)wh));

        painter.setPen(penGreen);
        painter.drawLine(x, wh - 1, x, wh - 1 -
            (int)((float)getHistValue(mChannelGreen,
            x, ww) / (float)mMaxAll * (float)wh));

        painter.setPen(penBlue);
        painter.drawLine(x, wh - 1, x, wh - 1 -
            (int)((float)getHistValue(mChannelBlue,
            x, ww) / (float)mMaxAll * (float)wh));
    }
}
}
