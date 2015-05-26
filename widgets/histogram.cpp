#include <QPainter>
#include <QPen>
#include <QColor>
#include <QDebug>

#include "histogram.h"


Histogram::Histogram(QWidget *parent) : QWidget(parent)
{
    setMinimumHeight(120);
    setMaximumHeight(120);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
}

void Histogram::setImageData(const QImage &image)
{
    // the default BIN_SIZE = 256
    //set all values to 0
    memset(mChannelRed,0,sizeof(unsigned long)*BIN_SIZE);
    memset(mChannelBlue,0,sizeof(unsigned long)*BIN_SIZE);
    memset(mChannelGreen,0,sizeof(unsigned long)*BIN_SIZE);

    mMaxRed = mMaxGreen = mMaxBlue = mMaxAll = 0;

    // first do uniform distribution (linear transform)

    if (image.depth() == 32)
    {
        const QRgb *rgb = (QRgb*)image.constBits();
        int count = image.byteCount() / 4;
        for(int i = 0; i<count; i++)
        {
            unsigned int red = qRed(rgb[i]);
            unsigned int green = qGreen(rgb[i]);
            unsigned int blue = qBlue(rgb[i]);
            unsigned int alpha = qAlpha(rgb[i]);

            mChannelRed[red]++;
            mChannelGreen[green]++;
            mChannelBlue[blue]++;

            // use green as the average luminance.
            // use this value to scale the y axis of the histogram when painting
            if (mMaxAll < mChannelGreen[green])
                mMaxAll = mChannelGreen[green];

        }
        update();
    }
}

void Histogram::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    int ww = width(); // widget width
    int wh = height(); // widget height
    QPen penRed = QPen(QColor(Qt::red).darker(120));
    QPen penGreen = QPen(QColor(Qt::green).darker(120));
    QPen penBlue = QPen(QColor(Qt::blue).darker(120));

    painter.setCompositionMode(QPainter::CompositionMode_Plus);
    for(int x = 0; x<256; x++)
    {
        painter.setPen(penRed);
        painter.drawLine(x,wh-1,x,wh-(int)((double)mChannelRed[x]/(double)mMaxAll*(double)wh));

        painter.setPen(penGreen);
        painter.drawLine(x,wh-1,x,wh-(int)((double)mChannelGreen[x]/(double)mMaxAll*(double)wh));

        painter.setPen(penBlue);
        painter.drawLine(x,wh-1,x,wh-(int)((double)mChannelBlue[x]/(double)mMaxAll*(double)wh));

    }
}

