#include <QPainter>
#include <QPen>
#include <QColor>
#include <QDebug>

#include "histogram.h"

Histogram::Histogram(QWidget* parent) : QWidget(parent)
{
    setMinimumHeight(120);
    setMaximumHeight(120);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    memset(mChannelRed, 0, sizeof(unsigned long) * BIN_SIZE);
    memset(mChannelBlue, 0, sizeof(unsigned long) * BIN_SIZE);
    memset(mChannelGreen, 0, sizeof(unsigned long) * BIN_SIZE);

    mMaxAll = 0;
}

//void Histogram::setImageData(const PhotoData &image)
void Histogram::setImageData(const Image& image)
{
    mImage = image;
    recalculate();
}

void Histogram::recalculate()
{
    // the default BIN_SIZE = 256
    //set all values to 0
    memset(mChannelRed, 0, sizeof(unsigned long) * BIN_SIZE);
    memset(mChannelBlue, 0, sizeof(unsigned long) * BIN_SIZE);
    memset(mChannelGreen, 0, sizeof(unsigned long) * BIN_SIZE);

    mMaxAll = 0;

    // first do uniform distribution (linear transform)
    // TODO: do we need to count all or just 1/2 or 1/4 of the pic (skip 2 or 4)

    int count = mImage.width();

    for (int j = 0; j < mImage.height(); j++)
    {
        float* pixels = mImage.scanLine(j);

        for (int i = 0; i < count; i++)
        {
            float red   = pixels[i * 3 + 2];
            float green = pixels[i * 3 + 1];
            float blue  = pixels[i * 3 + 0];

            int   factor = BIN_SIZE;

            // TODO: consider how to handle values outside range
            if (red > 0.0 && red < 1.0)
                mChannelRed[(int)(red * factor)]++;

            if (green > 0.0 && green < 1.0)
                mChannelGreen[(int)(green * factor)]++;

            if (blue > 0.0 && blue < 1.0)
                mChannelBlue[(int)(blue * factor)]++;

            // use green as the average luminance.
            // use this value to scale the y axis of the histogram when painting

            if (green > 0.0 && green < 1.0)
                if (mMaxAll < mChannelGreen[(int)((double)green * factor)])
                    mMaxAll = mChannelGreen[(int)((double)green * factor)];

        }
    }
    update();
}

void Histogram::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    int      ww       = width(); // widget width
    int      wh       = height(); // widget height
    QPen     penRed   = QPen(QColor(Qt::red).darker(120));
    QPen     penGreen = QPen(QColor(Qt::green).darker(120));
    QPen     penBlue  = QPen(QColor(Qt::blue).darker(120));

    painter.setCompositionMode(QPainter::CompositionMode_Plus);

    if (mMaxAll == 0)
        return;

    for (int x = 0; x < 256; x++)
    {
        painter.setPen(penRed);
        painter.drawLine(x, wh - 1, x, wh - (int)((double)mChannelRed[x] / (double)mMaxAll * (double)wh));

        painter.setPen(penGreen);
        painter.drawLine(x, wh - 1, x, wh - (int)((double)mChannelGreen[x] / (double)mMaxAll * (double)wh));

        painter.setPen(penBlue);
        painter.drawLine(x, wh - 1, x, wh - (int)((double)mChannelBlue[x] / (double)mMaxAll * (double)wh));
    }
}
