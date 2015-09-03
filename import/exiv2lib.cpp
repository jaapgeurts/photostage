#include <QDebug>
#include <QFileInfo>
#include <math.h>

#include "exivutils.h"

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

template<typename T>
static inline T max3(T a, T b, T c)
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
            uint16_t                 bytecount;
            uint16_t                 _unknown[24];
            struct WhiteBalanceEntry WhiteBalanceTable[10];
        }
        V1;

        // this version is for the 1D Mark II and the 1Ds Mark II
        struct V2
        {
            //    uint16_t bytecount1;
            //    uint16_t values[4]; // Some RGGB values
            uint16_t                 bytecount;
            uint16_t                 _unknown[31];
            struct WhiteBalanceEntry WhiteBalanceTable[10];
        }
        V2;
        // this version is for the G10 and the Powershot S110
        struct V3
        {
            uint16_t                 bytecount;
            uint16_t                 _unknown[70];
            struct WhiteBalanceEntry WhiteBalanceTable[10];
        }
        V3;
        // this is the default for canon
        struct V4
        {
            uint16_t                 bytecount;
            uint16_t                 _unknown[62];
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
            uint16_t                 version;
            uint16_t                 _unknown[62];
            struct WhiteBalanceEntry WhiteBalanceTable[10];
        }
        V7;
    };
};

Exiv2Lib::Exiv2Lib()
{
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
        qDebug() << "Exiv2 error reading file" << e.what();
        return false;
    }

    return populateExifInfo();
}

bool Exiv2Lib::openData(const QByteArray& data)
{
    try
    {
        mImageFile = ImageFactory::open((const byte*)data.constData(), (long)data.length());

        return populateExifInfo();
    }
    catch (BasicError<char> e)
    {
        qDebug() << "Exiv2 error reading data" << e.what();
        return false;
    }
    return false;
}

bool Exiv2Lib::populateExifInfo()
{
    if (mImageFile.get() == NULL && mImageFile.get()->good())
    {
        qDebug() << "Exiv2:: no image or not good";
        return false;
    }

    mImageFile->readMetadata();

    ExifData& data = mImageFile->exifData();

    mExifInfo.make              = getExifValue<QString>(data, "Exif.Image.Make");
    mExifInfo.model             = getExifValue<QString>(data, "Exif.Image.Model");
    mExifInfo.exposureTime      = getExifValue<float>(data, "Exif.Photo.ExposureTime");
    mExifInfo.aperture          = getExifValue<float>(data, "Exif.Photo.FNumber");
    mExifInfo.isoSpeed          = getExifValue<uint8_t>(data, "Exif.Photo.ISOSpeedRatings");
    mExifInfo.dateTimeOriginal  = getExifValue<QDateTime>(data, "Exif.Photo.DateTimeOriginal");
    mExifInfo.dateTimeDigitized = getExifValue<QDateTime>(data, "Exif.Photo.DateTimeDigitized");
    mExifInfo.flash             = getExifValue<bool>(data, "Exif.Photo.Flash");
    mExifInfo.focalLength       = getExifValue<float>(data, "Exif.Photo.FocalLength");
    Nullable<ExifInfo::Rotation> rot = getExifValue<ExifInfo::Rotation>(data, "Exif.Image.Orientation");
    mExifInfo.rotation  = rot == nullptr ? ExifInfo::NotRotated : *rot;
    mExifInfo.copyright = getExifValue<QString>(data, "Exif.Image.Copyright");
    mExifInfo.artist    = getExifValue<QString>(data, "Exif.Image.Artist");
    Nullable<uint16_t> w = getExifValue<uint16_t>(data, "Exif.Image.ImageWidth");
    Nullable<uint16_t> h = getExifValue<uint16_t>(data, "Exif.Image.ImageLength");
    mExifInfo.width  = w == nullptr ? 0 : *w;
    mExifInfo.height = h == nullptr ? 0 : *h;

    if (mExifInfo.make != nullptr)
    {
        if (mExifInfo.make->startsWith("Canon"))
            readCanonNotes(data);
        else if (mExifInfo.make->startsWith("NIKON"))
            readNikonNotes(data);
    }

    return true;
}

ExifInfo Exiv2Lib::data()
{
    return mExifInfo;
}

void Exiv2Lib::readCanonNotes(ExifData& data)
{
    uint16_t lens[3];

    if (getExifBytes(data, "Exif.CanonCs.Lens", (uint8_t*)lens,
        3 * sizeof(uint16_t)))
    {
        QString lfl = QString::number(lens[0]);
        QString sfl = QString::number(lens[1]);
        /*
         * for now forget the aperture calculation encoding of the exif fields.
                long val = getExifValue<int16_t>(data,
                                              "Exif.CanonCs.MinAperture", 0);

                                              // code copied from Exiv2
                    // temporarily remove sign
                    int sign = 1;

                    if (val < 0)
                    {
                        sign = -1;
                        val  = -val;
                    }
                    // remove fraction
                    float frac = static_cast<float>(val & 0x1f);
                    val -= long(frac);

                    // convert 1/3 (0x0c) and 2/3 (0x14) codes
                    if (frac == 0x0c)
                    {
                        frac = 32.0f / 3;
                    }
                    else if (frac == 0x14)
                    {
                        frac = 64.0f / 3;
                    }
                    float apex = sign * (val + frac) / 32.0f;


                QString mina =
                    QString::number(log(2.0f) * apex /2);
                QString maxa =
                    QString::number(getExifValue<int16_t>(data,
                        "Exif.CanonCs.MaxAperture", 0));
         */
        mExifInfo.lensName = QString("%1-%2mm").arg(sfl, lfl);
    }

    setWhiteBalanceCoeffsCanon(data, mExifInfo.rgbCoeffients);
}

void Exiv2Lib::readNikonNotes(ExifData& data)
{
    double lens[4];

    if (getExifBytes(data, "Exif.Nikon3.Lens", (uint8_t*)lens,
        4 * sizeof(double)))
    {
        mExifInfo.lensName =
            QString("%1-%2 ƒ/%3-%4").arg(lens[0], 0, 'f', 0).
            arg(lens[1], 0, 'f', 0).arg(lens[2], 0, 'f', 0).
            arg(lens[3], 0, 'f', 0);
    }
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
        if (*mExifInfo.model == "NIKON D300")
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

    if (*mExifInfo.model == "Canon EOS 300D DIGITAL" ||
        *mExifInfo.model == "Canon EOS DIGITAL REBEL")
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

        if (*mExifInfo.model == "Canon EOS 350D DIGITAL")     // || EOS 20D
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
        else if (*mExifInfo.model == "Canon EOS 1D Mark II")     // || 1Ds Mark II
        {
            qDebug() << "Whitebalance info unavailable";
        }
        else if (*mExifInfo.model == "Canon G10")
        {
            qDebug() << "Whitebalance info unavailable";
        }
        else if (*mExifInfo.model == "Canon PowerShot S30")
        {
            wb[0] = colorData->V5.WhiteBalanceTable[WB_AsShot].GRBG[1];
            wb[1] =
                (colorData->V5.WhiteBalanceTable[WB_AsShot].GRBG[0] +
                colorData->V5.WhiteBalanceTable[WB_AsShot].GRBG[3]) / 2;
            wb[2] = colorData->V5.WhiteBalanceTable[WB_AsShot].GRBG[2];
        }
        else if (*mExifInfo.model == "Canon PowerShot S110")
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
