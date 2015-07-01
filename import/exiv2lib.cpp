#include <QDebug>
#include <QFileInfo>

#include "exiv2lib.h"
#include "constants.h"

#define WB_AsShot      0
#define WB_Auto        1
#define WB_Daylight    2
#define WB_Shade       3
#define WB_Cloudy      4
#define WB_Tungsten    5
#define WB_Fluorescent 6
#define WB_Flash       7
#define WB_Custom1     8
#define WB_Custom2     9

using namespace Exiv2;

static inline float max3(float a, float b, float c)
{
    if (a > b)
        return a > c ? a : c;
    else
        return b > c ? b : c;
}

namespace PhotoStage
{
struct __attribute__((packed)) WhiteBalanceEntry
{
    uint16_t RGGB[4];
    uint16_t Temperature;
};

struct __attribute__((packed)) Canon_ColorData
{
    union
    {
        // This version for the 20D & 350D
        struct V1
        {
            uint16_t bytecount;
            uint16_t _unknown[24];
            struct WhiteBalanceEntry WhiteBalanceTable[10];
        }
        V1;

        // this version is for the 1D Mark II and the 1Ds Mark II
        struct V2
        {
            //    uint16_t bytecount1;
            //    uint16_t values[4]; // Some RGGB values
            uint16_t bytecount;
            uint16_t _unknown[31];
            struct WhiteBalanceEntry WhiteBalanceTable[10];
        }
        V2;
        // this version is for the G10 and the Powershot S110
        struct V3
        {
            uint16_t bytecount;
            uint16_t _unknown[70];
            struct WhiteBalanceEntry WhiteBalanceTable[10];
        }
        V3;
        // this is the default for canon
        struct V4
        {
            uint16_t bytecount;
            uint16_t _unknown[62];
            struct WhiteBalanceEntry WhiteBalanceTable[10];
        }
        V4;
        struct V5
        {
            uint16_t bytecount;
            uint16_t _unknown[49];
            struct
            {
                uint16_t GRBG[4];
            }
            WhiteBalanceTable[3];
        }
        V5;
        struct V7
        {
            uint16_t version;
            uint16_t _unknown[62];
            struct WhiteBalanceEntry WhiteBalanceTable[10];
        }
        V7;
    };
};

Exiv2Lib::Exiv2Lib()
{
}

template<typename T>
T Exiv2Lib::getExifValue(ExifData& data, QString key, T defValue)
{
    qDebug() << "Template function getExifValue() not defined for type" <<
        typeid(T).name();
    return defValue;
}

template<>
QString Exiv2Lib::getExifValue(ExifData& data, QString key, QString defValue)
{
    ExifData::const_iterator pos;

    pos = data.findKey(ExifKey(key.toStdString()));

    if (pos != data.end())
        return QString::fromStdString(pos->toString());
    return defValue;
}

template<>
float Exiv2Lib::getExifValue(ExifData& data, QString key, float defValue)
{
    ExifData::const_iterator pos;

    pos = data.findKey(ExifKey(key.toStdString()));

    if (pos != data.end())
        return pos->toFloat();
    return defValue;
}

template<>
long Exiv2Lib::getExifValue(ExifData& data, QString key, long defValue)
{
    ExifData::const_iterator pos;

    pos = data.findKey(ExifKey(key.toStdString()));

    if (pos != data.end())
        return pos->toLong();
    return defValue;
}

template<>
int Exiv2Lib::getExifValue(ExifData& data, QString key, int defValue)
{
    ExifData::const_iterator pos;

    pos = data.findKey(ExifKey(key.toStdString()));

    if (pos != data.end())
        return (int)pos->toLong();
    return defValue;
}

template<>
bool Exiv2Lib::getExifValue(ExifData& data, QString key, bool defValue)
{
    ExifData::const_iterator pos;

    pos = data.findKey(ExifKey(key.toStdString()));

    if (pos != data.end())
        return pos->toLong() == 1;
    return defValue;
}

template<>
ExifInfo::Rotation Exiv2Lib::getExifValue(ExifData& data,
    QString key,
    ExifInfo::Rotation defValue)
{
    ExifData::const_iterator pos;

    pos = data.findKey(ExifKey(key.toStdString()));

    if (pos != data.end())
        return (ExifInfo::Rotation)pos->toLong();
    return defValue;
}

template<>
QDateTime Exiv2Lib::getExifValue(ExifData& data, QString key,
    QDateTime defValue)
{
    ExifData::const_iterator pos;

    pos = data.findKey(ExifKey(key.toStdString()));

    if (pos != data.end())
    {
        QString   datetime = QString::fromStdString(pos->toString());
        QDateTime result   = QDateTime::fromString(datetime, Qt::ISODate);

        if (result.isValid())
        {
            return result;
        }
        else
        {
            qDebug() << "Can't parse date:" << datetime;
        }
    }
    return defValue;
}

bool Exiv2Lib::openFile(const QString& path)
{
    QFileInfo fi(path);

    if (!fi.isFile())
    {
        qDebug() << "Error exiv2lib:" << path << "is not a regular file";
        return false;
    }

    try
    {
        mImageFile = ImageFactory::open(path.toStdString());
    }
    catch (BasicError<char> e)
    {
        qDebug() << "Error Exiv2" << e.what();
        return false;
    }

    if (mImageFile.get() == NULL)
    {
        qDebug() << "Exiv2:: no image";
        return false;
    }
    mImageFile->readMetadata();

    ExifData& data = mImageFile->exifData();

    mExifInfo.make = getExifValue<QString>(data,
            "Exif.Image.Make",
            "");
    mExifInfo.model = getExifValue<QString>(data,
            "Exif.Image.Model",
            "");
    mExifInfo.exposureTime = getExifValue<float>(data,
            "Exif.Photo.ExposureTime",
            0.0f);

    mExifInfo.aperture = getExifValue<float>(data,
            "Exif.Photo.FNumber",
            0.0f);
    mExifInfo.isoSpeed = getExifValue<int>(data,
            "Exif.Photo.ISOSpeedRatings",
            0);
    mExifInfo.dateTimeOriginal = getExifValue<QDateTime>(data,
            "Exif.Photo.DateTimeOriginal",
            QDateTime());
    mExifInfo.dateTimeDigitized = getExifValue<QDateTime>(data,
            "Exif.Photo.DateTimeDigitized",
            QDateTime());
    mExifInfo.flash = getExifValue<bool>(data,
            "Exif.Photo.Flash",
            false);
    mExifInfo.focalLength = getExifValue<float>(data,
            "Exif.Photo.FocalLength",
            0.0f);
    mExifInfo.lensName = getExifValue<QString>(data,
            "Exif.Photo.LensModel",
            "Unknown lens");
    mExifInfo.rotation = getExifValue<ExifInfo::Rotation>(data,
            "Exif.Image.Orientation",
            ExifInfo::NotRotated);
    mExifInfo.copyright = getExifValue<QString>(data,
            "Exif.Image.Copyright",
            "");
    mExifInfo.artist = getExifValue<QString>(data,
            "Exif.Image.Artist",
            "");

    if (mExifInfo.make.startsWith("Canon"))
        readCanonNotes(data);
    else if (mExifInfo.make.startsWith("NIKON"))
        readNikonNotes(data);

    return true;
}

ExifInfo Exiv2Lib::data()
{
    return mExifInfo;
}

void Exiv2Lib::readCanonNotes(ExifData& data)
{
    setWhiteBalanceCoeffsCanon(data, mExifInfo.rgbCoeffients);
}

void Exiv2Lib::readNikonNotes(ExifData& data)
{
    setWhiteBalanceCoeffsNikon(data, mExifInfo.rgbCoeffients);
}

void Exiv2Lib::setWhiteBalanceCoeffsNikon(ExifData& data, float wb[3])
{
    ExifData::const_iterator pos;

    /*
     * This seems to be unnecessary as the multipliers are also
     * available as rational numbers in the WB_RBLevels tag
     */
    /*
       pos = data.findKey(ExifKey("Exif.Nikon3.Version"));

       int makerNoteVersion = -1;

       if (pos != data.end())
       {
           char*   buf =  new char[pos->size()];
           pos->copy((unsigned char*)buf, littleEndian);
           QString val(buf);
           qDebug() << val;
           delete [] buf;

           makerNoteVersion = val.toInt();
       }
       qDebug() << "Nikon makenote version:" << makerNoteVersion;

       if (makerNoteVersion >= 200)
       {
        qDebug() << "Decrypt Nikon Color Balance";
       }

     */
    pos = data.findKey(ExifKey("Exif.Nikon3.WB_RBLevels"));

    if (pos != data.end())
    {
        if (mExifInfo.model == "NIKON D300")
        {
            wb[0] = 1.0f;
            wb[1] = 1.0f;
            wb[2] = 1.0f;
            //            wb[0] = colorData->V3.WhiteBalanceTable[WB_AsShot].RGGB[0];
            //            wb[1] =
            //                ((colorData->V1.WhiteBalanceTable[WB_AsShot].RGGB[1] +
            //                colorData->V1.WhiteBalanceTable[WB_AsShot].RGGB[2]) / 2);
            //            wb[2] = colorData->V1.WhiteBalanceTable[WB_AsShot].RGGB[3];
            qDebug() << "White balance for model Nikon D300";

            if (pos != data.end())
            {
                wb[0] = pos->toFloat(0);
                wb[1] = 1.0f;
                wb[2] = pos->toFloat(1);
                qDebug() << "Nikon: RGB" << wb[0] << "," << wb[1] << "," <<
                    wb[2];
            }
        }
        float mx = max3(wb[0], wb[1], wb[2]);

        wb[0] /= mx;
        wb[1] /= mx;
        wb[2] /= mx;
    }
    else
    {
        qDebug () << "Nikon: Color balance data not available";
        wb[0] = 1.0f;
        wb[1] = 1.0f;
        wb[2] = 1.0f;
    }
}

void Exiv2Lib::setWhiteBalanceCoeffsCanon(ExifData& data, float wb[3])
{
    ExifData::const_iterator pos;

    if (mExifInfo.model == "Canon EOS 300D DIGITAL" ||
        mExifInfo.model == "Canon EOS DIGITAL REBEL")
    {
        pos = data.findKey(ExifKey("Exif.Canon.WhiteBalanceTable"));

        if (pos != data.end())
        {
            qDebug() << "Reading EOS 300D tags";
            uint16_t* cdata = new uint16_t[pos->size() / 2];
            pos->copy((unsigned char*)cdata, littleEndian);

            wb[0] = cdata[1];
            wb[1] = (cdata[2] + cdata[3]) / 2;
            wb[2] = cdata[4];
            float mx = max3(wb[0], wb[1], wb[2]);

            wb[0] /= mx;
            wb[1] /= mx;
            wb[2] /= mx;

            delete [] cdata;
            return;
        }
    }

    pos = data.findKey(ExifKey("Exif.Canon.ColorData"));

    if (pos != data.end())
    {
        uint8_t*                cdata = new uint8_t[pos->size()];

        struct Canon_ColorData* colorData;
        pos->copy(cdata, littleEndian);
        colorData = (struct Canon_ColorData*)cdata;

        if (mExifInfo.model == "Canon EOS 350D DIGITAL")     // || EOS 20D
        {
            // optimize with bitshifts later
            //                                    wb[0] = colorData->V1.WhiteBalanceTable[WB_AsShot].RGGB[2];
            //                                    wb[1] = ((colorData->V1.WhiteBalanceTable[WB_AsShot].RGGB[1]+colorData->V1.WhiteBalanceTable[WB_AsShot].RGGB[3])/2);
            //                                    wb[2] = colorData->V1.WhiteBalanceTable[WB_AsShot].RGGB[0];
            wb[0] = colorData->V1.WhiteBalanceTable[WB_AsShot].RGGB[0];
            wb[1] =
                ((colorData->V1.WhiteBalanceTable[WB_AsShot].RGGB[1] +
                colorData->V1.WhiteBalanceTable[WB_AsShot].RGGB[2]) / 2);
            wb[2] = colorData->V1.WhiteBalanceTable[WB_AsShot].RGGB[3];

            qDebug() << "WB EOS 350D DIGITAL RGGB" << wb[0] << "," << wb[1] <<
                "," << wb[2];
        }
        else if (mExifInfo.model == "Canon EOS 1D Mark II")     // || 1Ds Mark II
        {
            qDebug() << "Whitebalance info unavailable";
        }
        else if (mExifInfo.model == "Canon G10")
        {
            qDebug() << "Whitebalance info unavailable";
        }
        else if (mExifInfo.model == "Canon PowerShot S30")
        {
            wb[0] = colorData->V5.WhiteBalanceTable[WB_AsShot].GRBG[1];
            wb[1] =
                (colorData->V5.WhiteBalanceTable[WB_AsShot].GRBG[0] +
                colorData->V5.WhiteBalanceTable[WB_AsShot].GRBG[3]) / 2;
            wb[2] = colorData->V5.WhiteBalanceTable[WB_AsShot].GRBG[2];
        }
        else if (mExifInfo.model == "Canon PowerShot S110")
        {
            wb[0] =  colorData->V3.WhiteBalanceTable[WB_AsShot].RGGB[0];
            wb[1] = (colorData->V3.WhiteBalanceTable[WB_AsShot].RGGB[1] +
                colorData->V3.WhiteBalanceTable[WB_AsShot].RGGB[2]) / 2;
            wb[2] =  colorData->V3.WhiteBalanceTable[WB_AsShot].RGGB[3];
        }
        else     // attempt to read at the default position at V4
        {
            qDebug() <<
                "Model unknown, Parsing whitebalance using canon default format";
            qDebug() << "Color data version: " << colorData->V7.version;
            // optimize with bitshifs later
            wb[0] = colorData->V7.WhiteBalanceTable[WB_AsShot].RGGB[0];
            wb[1] =
                (colorData->V7.WhiteBalanceTable[WB_AsShot].RGGB[1] +
                colorData->V7.WhiteBalanceTable[WB_AsShot].RGGB[2]) / 2;
            wb[2] = colorData->V7.WhiteBalanceTable[WB_AsShot].RGGB[3];
        }

        float mx = max3(wb[0], wb[1], wb[2]);

        wb[0] /= mx;
        wb[1] /= mx;
        wb[2] /= mx;

        delete [] cdata;
    }
    else
    {
        //qDebug() << "no such exif key";

        wb[0] = 1.0;
        wb[1] = 1.0;
        wb[2] = 1.0;
    }
    //    qDebug() << "WB RGGB Multipliers" <<wb[0] <<","<<wb[1]<<","<<wb[2];
}

QImage Exiv2Lib::getPreview()
{
    PreviewManager        loader(*mImageFile);
    PreviewPropertiesList list = loader.getPreviewProperties();

    if (list.empty())
    {
        qDebug() << "Image contains no thumbnail";
        return QImage();
    }

    PreviewProperties selected;
    // Take the last image in the list.
    // For Canon raw pictures, the 2nd preview has no white balance applied.
    // see http://lclevy.free.fr/cr2/
    //     section 2.6 IFD #2
    selected = list.back();

    // FIXME: crash hapened here when reading the QImage.
    // not quite sure what caused it

    // load the thumbnail
    PreviewImage         thumbnail = loader.getPreviewImage(selected);

    const unsigned char* tmp  = thumbnail.pData();
    size_t               size = thumbnail.size();

    QImage               thumb;

    thumb.loadFromData(tmp, size);

    // Read the EXIF orientation flag and rotate the image accordingly.
    ExifData&                data = mImageFile->exifData();
    ExifData::const_iterator pos;
    pos = data.findKey(ExifKey("Exif.Image.Orientation"));

    // TODO: should flip/rotate after resize
    if (pos != data.end())
    {
        qDebug() << "rotation" << pos->toLong();
        QTransform rotate;

        switch (pos->toLong())
        {
            case 1:     // no rotation needed
                break;

            case 2:     // flip over y
                thumb = thumb.mirrored(true, false);
                break;

            case 3:     // rotate 180 (or flip x flip y)
                thumb = thumb.mirrored(true, true);
                break;

            case 4:     // flip over x
                thumb = thumb.mirrored(false, true);
                break;

            case 5:     // rotate 90 CW & flip over y
                rotate.rotate(90);
                rotate.rotate(180, Qt::YAxis);
                thumb = thumb.transformed(rotate);
                break;

            case 6:     // rotate 90 CW
                rotate.rotate(90);
                thumb = thumb.transformed(rotate);
                break;

            case 7:     // rotate 90 CCW flip over y
                rotate.rotate(-90);
                rotate.rotate(180, Qt::YAxis);
                thumb = thumb.transformed(rotate);
                break;

            case 8:     // rotate 90 CCW
                rotate.rotate(-90);
                thumb = thumb.transformed(rotate);
                break;
        }
    }
    return thumb;
}
}
