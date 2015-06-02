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

    memset(mChannelRed,0,sizeof(unsigned long)*BIN_SIZE);
    memset(mChannelBlue,0,sizeof(unsigned long)*BIN_SIZE);
    memset(mChannelGreen,0,sizeof(unsigned long)*BIN_SIZE);

    mMaxAll = 0;

}

void Histogram::setImageData(const PhotoData &image)
{
    // the default BIN_SIZE = 256
    //set all values to 0
    memset(mChannelRed,0,sizeof(unsigned long)*BIN_SIZE);
    memset(mChannelBlue,0,sizeof(unsigned long)*BIN_SIZE);
    memset(mChannelGreen,0,sizeof(unsigned long)*BIN_SIZE);

    mMaxAll = 0;

    // first do uniform distribution (linear transform)

    double factor = (double)BIN_SIZE / (double)65535;
    const uint16_t* pixels = image.constData();
    int count = image.length() / NUM_CHANNELS;
    for(int i = 0; i<count-3; i+=3)
    {
        uint16_t red = pixels[i];
        uint16_t green = pixels[i+1];
        uint16_t blue = pixels[i+2];

        mChannelRed[(int)((double)red*factor)]++;
        mChannelGreen[(int)((double)green*factor)]++;
        mChannelBlue[(int)((double)blue*factor)]++;

        // use green as the average luminance.
        // use this value to scale the y axis of the histogram when painting
        if (mMaxAll < mChannelGreen[(int)((double)green*factor)])
            mMaxAll = mChannelGreen[(int)((double)green*factor)];

    }
    qDebug() << "largest green value:"<<mMaxAll;
    update();
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
    if (mMaxAll == 0)
        return;
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

