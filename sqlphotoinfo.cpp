#include "sqlphotoinfo.h"

#include <QDebug>

SqlPhotoInfo::SqlPhotoInfo()
{

}

SqlPhotoInfo::SqlPhotoInfo(const SqlPhotoInfo &info)
{
    *this = info;
}

SqlPhotoInfo::SqlPhotoInfo(const QImage &image, const QString &filename, long long id)
{
    this->image = image;
    this->fileName = filename;
    this->id = id;
}

SqlPhotoInfo::~SqlPhotoInfo()
{

}


void SqlPhotoInfo::setRating(int rating)
{
    if (rating<0 || rating > 5) {
        qDebug() << "Invalid rating:" << rating;
        return;
    }
    mRating = rating;
}


int SqlPhotoInfo::rating()
{
    return mRating;
}


void SqlPhotoInfo::setColorLabel(SqlPhotoInfo::ColorLabel label)
{
    mColorLabel = label;
}


SqlPhotoInfo::ColorLabel SqlPhotoInfo::colorLabel()
{
    return mColorLabel;
}


void SqlPhotoInfo::setFlag(SqlPhotoInfo::Flag flag)
{
    mFlag = flag;
}


SqlPhotoInfo::Flag SqlPhotoInfo::flag()
{
    return mFlag;
}
