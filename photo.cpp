#include "photo.h"

#include <QDir>
#include <QDebug>

Photo::Photo()
{

}

Photo::Photo(const Photo &info)
{
    *this = info;
}

Photo::Photo(const QImage &image, const QString &filename, long long id)
{
    this->image = image;
    this->fileName = filename;
    this->id = id;
}

Photo::Photo(QSqlQuery &q)
{
    id = q.value(0).toInt();
    QString filename = q.value(1).toString();
    QString path = q.value(2).toString();
    fileName = path + QDir::separator() + filename;
    if (q.value(3).isNull())
        setRating(0);
    else
        setRating(q.value(3).toInt());
    setColorLabel((Photo::ColorLabel)q.value(4).toInt());
    setFlag((Photo::Flag)q.value(5).toInt());
}

Photo::~Photo()
{

}


void Photo::setRating(int rating)
{
    if (rating<0 || rating > 5) {
        qDebug() << "Invalid rating:" << rating;
        return;
    }
    mRating = rating;
}


int Photo::rating()
{
    return mRating;
}


void Photo::setColorLabel(Photo::ColorLabel label)
{
    mColorLabel = label;
}


Photo::ColorLabel Photo::colorLabel()
{
    return mColorLabel;
}


void Photo::setFlag(Photo::Flag flag)
{
    mFlag = flag;
}


Photo::Flag Photo::flag()
{
    return mFlag;
}
