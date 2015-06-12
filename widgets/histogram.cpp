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

    int count = mImage.width();

    for (int j = 0; j < mImage.height(); j++)
    {
        float* pixels = mImage.scanLine(j);

        for (int i = 0; i < count - 3; i += 3)
        {
            float red   = pixels[i + 2];
            float green = pixels[i + 1];
            float blue  = pixels[i + 0];

            int   factor = BIN_SIZE;

            mChannelRed[(int)(red * factor)]++;
            mChannelGreen[(int)(green * factor)]++;
            mChannelBlue[(int)(blue * factor)]++;

            // use green as the average luminance.
            // use this value to scale the y axis of the histogram when painting
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
