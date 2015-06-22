#include <QDebug>

#include "colortransform.h"

namespace PhotoStage
{
    // init static variables
    QHash<QString, ColorTransform> ColorTransform::mTransformCache;

    ColorTransform ColorTransform::getTransform(const QString& from,
        const QString& to,
        Format inFormat,
        Format outFormat)
    {
        QString key = from + to;


        if (mTransformCache.contains(key))
        {
            return mTransformCache.value(key);
        }
        else
        {
            ColorTransform transform(from, to, inFormat, outFormat);
            mTransformCache.insert(key, transform );
            return transform;
        }
    }

    ColorTransform::ColorTransform() :
        mHTransform(NULL)
    {
    }

    static void transform_delete(char* d)
    {
        cmsHTRANSFORM transform = (cmsHTRANSFORM)d;


        cmsDeleteTransform(transform);
    }

    ColorTransform::ColorTransform(const QString& from,
        const QString& to,
        Format inFormat,
        Format outFormat)
    {
        cmsHPROFILE hInProfile, hOutProfile;

        QString     fromProfile =
            "/Users/jaapg/Development/PhotoStage/PhotoStage/ICCProfiles/" +
            from +
            ".icc";
        QString toProfile =
            "/Users/jaapg/Development/PhotoStage/PhotoStage/ICCProfiles/" + to +
            ".icc";


        hInProfile  = cmsOpenProfileFromFile(
            fromProfile.toLocal8Bit().data(), "r");
        hOutProfile =
            cmsOpenProfileFromFile(toProfile.toLocal8Bit().data(), "r");

#define MY_TYPE (FLOAT_SH(1) | COLORSPACE_SH(PT_RGB) | CHANNELS_SH(3) | \
    BYTES_SH(4) | DOSWAP_SH(1))

        cmsUInt32Number inputFormat  = MY_TYPE;
        cmsUInt32Number outputFormat = MY_TYPE;

        if (inFormat == FORMAT_RGB32)
            inputFormat = TYPE_BGRA_8;

        if (outFormat == FORMAT_RGB32)
            outputFormat = TYPE_BGRA_8;

        mHTransform =
            QSharedPointer<char>((char*)cmsCreateTransform(hInProfile,
                inputFormat,
                hOutProfile,
                outputFormat,
                INTENT_PERCEPTUAL,
                0), transform_delete);

        cmsCloseProfile(hInProfile);
        cmsCloseProfile(hOutProfile);
    }

    ColorTransform::~ColorTransform()
    {
    }

    bool ColorTransform::isValid() const
    {
        return mHTransform != NULL;
    }

    Image ColorTransform::transformImage(const Image& inImage) const
    {
        //PhotoData result = PhotoData(inImage.size());

        //qDebug() << "Has alpha" << (inImage.hasAlphaChannel() ? "yes" : "no");

        Image  outImage(inImage.size());

        float* inbuf  = inImage.data();
        float* outbuf = outImage.data();


        cmsDoTransform(
            (cmsHTRANSFORM)mHTransform.data(), inbuf, outbuf,
            inImage.width() * inImage.height());

        return outImage;
    }

    QImage ColorTransform::transformToQImage(const Image& inImage) const
    {
        QImage outImage(inImage.size(), QImage::Format_RGB32);
        int    width  = outImage.width();
        int    height = outImage.height();


        for (int y = 0; y < height; y++)
        {
            const float* inLine  = inImage.scanLine(y);
            uint8_t*     outLine = outImage.scanLine(y);

            cmsDoTransform((cmsHTRANSFORM)mHTransform.data(), inLine, outLine,
                width);
            //        for (int x = 0; x < width; x++)
            //            for (int c = 0; c < 3; c++)
            //                outLine[x * 4 + c] = (uint8_t)(inLine[x * 3 + c] * 255.0);
        }
        return outImage;
    }

    QImage ColorTransform::transformQImage(const QImage& inImage) const
    {
        QImage outImage(inImage.size(), QImage::Format_RGB32);
        int    width  = outImage.width();
        int    height = outImage.height();


        for (int y = 0; y < height; y++)
        {
            const uint8_t* inLine  = inImage.constScanLine(y);
            uint8_t*       outLine = outImage.scanLine(y);

            cmsDoTransform((cmsHTRANSFORM)mHTransform.data(), inLine, outLine,
                width);
        }
        return outImage;
    }

    Image ColorTransform::transformFromQImage(const QImage& inImage) const
    {
        Image outImage(inImage.size());
        int   height = inImage.height();
        int   width  = inImage.width();


        for (int y = 0; y < height; y++)
        {
            const uint8_t* inLine  = inImage.constScanLine(y);
            float*         outLine = outImage.scanLine(y);
            cmsDoTransform((cmsHTRANSFORM)mHTransform.data(), inLine, outLine,
                width);
        }
        return outImage;
    }
}