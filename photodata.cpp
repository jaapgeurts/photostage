#include <QDebug>
#include "photodata.h"

PhotoData::PhotoData()
{
    mData.clear();
    qDebug() << "Setting mData to NULL";
}

PhotoData::PhotoData(const int w, const int h) : PhotoData()
{
    mWidth = w;
    mHeight = h;
    qDebug() << "Allocation image of size " << NUM_CHANNELS * w * h;
    mData = QSharedPointer<uint16_t>(new uint16_t[NUM_CHANNELS * w * h]);
}

PhotoData::PhotoData(const QSize &size) : PhotoData(size.width(),size.height())
{
}

PhotoData::~PhotoData()
{
}

uint16_t* PhotoData::data()
{
    return mData.data();
}

const uint16_t* PhotoData::constData() const
{
    return mData.data();
}

int PhotoData::length() const
{
    return mWidth * mHeight * NUM_CHANNELS;
}

uint16_t* PhotoData::scanLine(int i)
{
    return &(mData.data()[mWidth * i * NUM_CHANNELS]);
}

int PhotoData::width() const
{
    return mWidth;
}

int PhotoData::height() const
{
    return mHeight;
}