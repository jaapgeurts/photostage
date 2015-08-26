#ifndef PHOTOSTAGE_EXIVUTILS_H
#define PHOTOSTAGE_EXIVUTILS_H

#include <QDebug>

#include "exiv2lib.h"
#include "nullable.h"

template<typename T>
Nullable<T> getExifValue(Exiv2::ExifData&, const QString&)
{
    qDebug() << "Template function getExifValue() not defined for type" << typeid(T).name();
    return nullptr;
}

template<>
Nullable<QString> getExifValue(Exiv2::ExifData& data,
    const QString& key)
{
    try
    {
        Exiv2::ExifData::const_iterator pos;

        pos = data.findKey(Exiv2::ExifKey(key.toStdString()));

        if (pos != data.end() && pos->size() > 0)
            return Nullable<QString>(QString::fromStdString(pos->toString()));
    }
    catch (Exiv2::BasicError<char> e)
    {
        //  qDebug() << "No such Exif field" << key << "Debug:" << e.what();
    }
    return nullptr;
}

template<>
Nullable<float> getExifValue(Exiv2::ExifData& data, const QString& key)
{
    Exiv2::ExifData::const_iterator pos;

    try
    {
        pos = data.findKey(Exiv2::ExifKey(key.toStdString()));

        if (pos != data.end() && pos->size() > 0)
            return Nullable<float>(pos->toFloat());
    }
    catch (Exiv2::BasicError<char> e)
    {
        // qDebug() << "No such Exif field" << key << "Debug:" << e.what();
    }
    return nullptr;
}

template<>
Nullable<long> getExifValue(Exiv2::ExifData& data, const QString& key)
{
    Exiv2::ExifData::const_iterator pos;

    try
    {
        pos = data.findKey(Exiv2::ExifKey(key.toStdString()));

        if (pos != data.end() && pos->size() > 0)
            return Nullable<long>(pos->toLong());
    }
    catch (Exiv2::BasicError<char> e)
    {
        // qDebug() << "No such Exif field" << key << "Debug:" << e.what();
    }
    return nullptr;
}

template<>
Nullable<uint8_t> getExifValue(Exiv2::ExifData& data, const QString& key)
{
    Exiv2::ExifData::const_iterator pos;

    try
    {
        pos = data.findKey(Exiv2::ExifKey(key.toStdString()));

        if (pos != data.end() && pos->size() > 0)
            return Nullable<uint8_t>((uint8_t)pos->toLong());
    }
    catch (Exiv2::BasicError<char> e)
    {
        //qDebug() << "No such Exif field" << key << "Debug:" << e.what();
    }
    return nullptr;
}

template<>
Nullable<uint16_t> getExifValue(Exiv2::ExifData& data, const QString& key)
{
    Exiv2::ExifData::const_iterator pos;

    try
    {
        pos = data.findKey(Exiv2::ExifKey(key.toStdString()));

        if (pos != data.end() && pos->size() > 0)
            return Nullable<uint16_t>((uint16_t)pos->toLong());
    }
    catch (Exiv2::BasicError<char> e)
    {
        //  qDebug() << "No such Exif field" << key << "Debug:" << e.what();
    }
    return nullptr;
}

template<>
Nullable<int16_t> getExifValue(Exiv2::ExifData& data, const QString& key)
{
    Exiv2::ExifData::const_iterator pos;

    try
    {
        pos = data.findKey(Exiv2::ExifKey(key.toStdString()));

        if (pos != data.end() && pos->size() > 0)
            return Nullable<int16_t>((int16_t)pos->toLong());
    }
    catch (Exiv2::BasicError<char> e)
    {
        //qDebug() << "No such Exif field" << key << "Debug:" << e.what();
    }
    return nullptr;
}

template<>
Nullable<bool> getExifValue(Exiv2::ExifData& data, const QString& key)
{
    Exiv2::ExifData::const_iterator pos;

    try
    {
        pos = data.findKey(Exiv2::ExifKey(key.toStdString()));

        if (pos != data.end() && pos->size() > 0)
            return Nullable<bool>(pos->toLong() == 1);
    }
    catch (Exiv2::BasicError<char> e)
    {
        // qDebug() << "No such Exif field" << key << "Debug:" << e.what();
    }
    return nullptr;
}

template<>
Nullable<PhotoStage::ExifInfo::Rotation> getExifValue(Exiv2::ExifData& data, const QString& key)
{
    Exiv2::ExifData::const_iterator pos;

    try
    {
        pos = data.findKey(Exiv2::ExifKey(key.toStdString()));

        if (pos != data.end() && pos->size() > 0)
            return Nullable<PhotoStage::ExifInfo::Rotation>((PhotoStage::ExifInfo::Rotation)pos->toLong());
    }
    catch (Exiv2::BasicError<char> e)
    {
        // qDebug() << "No such Exif field" << key << "Debug:" << e.what();
    }
    return nullptr;
}

template<>
Nullable<QDateTime> getExifValue(Exiv2::ExifData& data, const QString& key)
{
    Exiv2::ExifData::const_iterator pos;

    try
    {
        pos = data.findKey(Exiv2::ExifKey(key.toStdString()));

        if (pos != data.end() && pos->size() > 0)
        {
            QString   datetime = QString::fromStdString(pos->toString());
            QDateTime result   = QDateTime::fromString(datetime, Qt::ISODate);

            if (result.isValid())
                return Nullable<QDateTime>(result);
            else
                qDebug() << "Can't parse date:" << datetime;
        }
    }
    catch (Exiv2::BasicError<char> e)
    {
        qDebug() << "No such Exif field" << key << "Debug:" << e.what();
    }
    return nullptr;
}

bool getExifBytes(Exiv2::ExifData& data, const QString& key, uint8_t* buf, size_t size)
{
    Exiv2::ExifData::const_iterator pos;

    try
    {
        pos = data.findKey(Exiv2::ExifKey(key.toStdString()));

        if (pos != data.end())
        {
            if (size != (size_t)pos->size())
                return false;
            pos->copy(buf, Exiv2::littleEndian);
            return true;
        }
    }
    catch (Exiv2::BasicError<char> e)
    {
        qDebug() << "No such Exif field" << key << "Debug:" << e.what();
    }
    return false;
}

#endif // PHOTOSTAGE_EXIVUTILS_H
