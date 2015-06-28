#include <QDebug>

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

void Exiv2Lib::openFile(const QString& path)
{
    try
    {
        mImageFile = ImageFactory::open(path.toStdString());
    }
    catch (BasicError<char> e)
    {
        qDebug() << "Exiv2::" << e.what();
        return;
    }

    if (mImageFile.get() == NULL)
    {
        qDebug() << "Exiv2:: no image";
        return;
    }
    mImageFile->readMetadata();

    ExifData&                data = mImageFile->exifData();
    ExifData::const_iterator pos;
    pos = data.findKey(ExifKey("Exif.Image.Make"));

    if (pos != data.end())
        mExifData.make = QString::fromStdString(pos->toString());

    // consider using Canon Model ID's
    pos = data.findKey(ExifKey("Exif.Image.Model"));

    if (pos != data.end())
    {
        // for canon chop off the name "Canon"
        mExifData.model = QString::fromStdString(pos->toString());

        if (mExifData.make.startsWith("Canon"))
        {
            mExifData.model = mExifData.model.remove(0, 6);
            setWhiteBalanceCoeffsCanon(data, mExifData.rgbCoeffients);
        }
        else if (mExifData.make.startsWith("NIKON"))
        {
            mExifData.model = mExifData.model.remove(0, 6);
            setWhiteBalanceCoeffsNikon(data, mExifData.rgbCoeffients);
        }
    }
    mExifData.thumbnail = loadImage();
}

ExifInfo Exiv2Lib::data()
{
    return mExifData;
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
        if (mExifData.model == "D300")
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

    pos = data.findKey(ExifKey("Exif.Canon.ColorData"));

    if (pos != data.end())
    {
        uint8_t*                cdata = new uint8_t[pos->size()];

        struct Canon_ColorData* colorData;
        pos->copy(cdata, littleEndian);
        colorData = (struct Canon_ColorData*)cdata;

        if (mExifData.model == "EOS 350D DIGITAL")     // || EOS 20D
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

            //            qDebug() << "WB EOS 350D DIGITAL RGGB" << wb[0] << ","<<wb[1]<<","<<wb[2];
        }
        else if (mExifData.model == "EOS 1D Mark II")     // || 1Ds Mark II
        {
            qDebug() << "Whitebalance info unavailable";
        }
        else if (mExifData.model == "G10")
        {
            qDebug() << "Whitebalance info unavailable";
        }
        else if (mExifData.model == "PowerShot S30")
        {
            uint16_t* s;
            s = &colorData->V5.WhiteBalanceTable[WB_AsShot].GRBG[0];
            //            qDebug() <<"****V5 WB values" << s[1] <<","<<s[0]<<","<<s[3]<<","<<s[2];

            // optimize with bitshifs later
            wb[0] = colorData->V5.WhiteBalanceTable[WB_AsShot].GRBG[1];
            wb[1] =
                (colorData->V5.WhiteBalanceTable[WB_AsShot].GRBG[0] +
                colorData->V5.WhiteBalanceTable[WB_AsShot].GRBG[3]) / 2;
            wb[2] = colorData->V5.WhiteBalanceTable[WB_AsShot].GRBG[2];
        }
        else if (mExifData.model == "PowerShot S110")
        {
            wb[0] = colorData->V3.WhiteBalanceTable[WB_AsShot].RGGB[0];
            wb[1] =
                (colorData->V3.WhiteBalanceTable[WB_AsShot].RGGB[1] +
                colorData->V3.WhiteBalanceTable[WB_AsShot].RGGB[2]) / 2;
            wb[2] = colorData->V3.WhiteBalanceTable[WB_AsShot].RGGB[3];

            qDebug() << "S110 Levels" << wb[0] << "," << wb[1] << "," <<
                wb[2];
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

            qDebug() << "WB RGGB Levels" << wb[0] << "," << wb[1] << "," <<
                wb[2];
        }

        float mx = max3(wb[0], wb[1], wb[2]);

        wb[0] /= mx;
        wb[1] /= mx;
        wb[2] /= mx;

        delete [] cdata;
    }
    else
    {
        // qDebug() << "no such exif key";

        wb[0] = 1.0;
        wb[1] = 1.0;
        wb[2] = 1.0;
    }
    //    qDebug() << "WB RGGB Multipliers" <<wb[0] <<","<<wb[1]<<","<<wb[2];
}

const QImage Exiv2Lib::loadImage()
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
