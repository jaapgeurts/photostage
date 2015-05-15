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
    this->rating = rating;
}


int SqlPhotoInfo::getRating()
{
    return rating;
}


void SqlPhotoInfo::setColorLabel(SqlPhotoInfo::ColorLabel label)
{
    this->color = label;
}


SqlPhotoInfo::ColorLabel SqlPhotoInfo::getColorLabel()
{
    return color;
}


void SqlPhotoInfo::setFlag(SqlPhotoInfo::Flag flag)
{
    this->flag = flag;
}


SqlPhotoInfo::Flag SqlPhotoInfo::getFlag()
{
    return flag;
}
