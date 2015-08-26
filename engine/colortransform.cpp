#include <QDir>
#include <QDebug>

#include "colortransform.h"

#include "platform.h"
#include "utils.h"

namespace PhotoStage
{
// init static variables
QHash<QString, ColorTransform> ColorTransform::mTransformCache;
QString                        ColorTransform::mMonitorProfilePath;

ColorTransform ColorTransform::getTransform(const QString& iden,
    const QString& from, const QString& to, Format inFormat, Format outFormat)
{
    if (mTransformCache.contains(iden))
    {
        return mTransformCache.value(iden);
    }
    else
    {
        ColorTransform transform(from, to, inFormat, outFormat);
        mTransformCache.insert(iden, transform );
        return transform;
    }
}

ColorTransform ColorTransform::getTransform(const QByteArray& srcProfile,
    const QString& to, Format inFormat, Format outFormat)
{
    ColorTransform transform(srcProfile, to, inFormat, outFormat);

    return transform;
}

QString ColorTransform::getMonitorProfilePath()
{
    // TODO: currently only return the profile for the primary monitor
    if (mMonitorProfilePath.isNull())
        mMonitorProfilePath = Platform::getMonitorProfilePath();

    return mMonitorProfilePath;
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

ColorTransform::ColorTransform(const QByteArray& srcProfile, const QString& to, Format inFormat, Format outFormat)
{
    cmsHPROFILE hInProfile, hOutProfile;

    QString     toProfile;

    if (to.at(0) == QDir::separator())
    {
        toProfile = to;
        qDebug() << "Loading monitor profile";
    }
    else
        toProfile =
            "/Users/jaapg/Development/PhotoStage/PhotoStage/ICCProfiles/" + to +
            ".icc";

    hInProfile  = cmsOpenProfileFromMem(srcProfile.constData(), srcProfile.length());
    hOutProfile = cmsOpenProfileFromFile(toProfile.toLocal8Bit().data(), "r");

#define MY_TYPE (FLOAT_SH(1) | COLORSPACE_SH(PT_RGB) | CHANNELS_SH(3) | \
    BYTES_SH(4) | DOSWAP_SH(1))

    cmsUInt32Number inputFormat  = MY_TYPE;
    cmsUInt32Number outputFormat = MY_TYPE;

    if (inFormat == FORMAT_RGB32)
        inputFormat = TYPE_BGRA_8;
    else if (inFormat == FORMAT_GRAYSCALE8)
        inputFormat = (COLORSPACE_SH(PT_RGB) | CHANNELS_SH(1) | BYTES_SH(1));

    if (outFormat == FORMAT_RGB32)
        outputFormat = TYPE_BGRA_8;

    mHTransform =
        QSharedPointer<char>((char*)cmsCreateTransform(hInProfile, inputFormat,
            hOutProfile, outputFormat, INTENT_PERCEPTUAL,
            0), transform_delete);

    cmsCloseProfile(hInProfile);
    cmsCloseProfile(hOutProfile);
}

ColorTransform::ColorTransform(const QString& from, const QString& to, Format inFormat, Format outFormat)
{
    cmsHPROFILE hInProfile, hOutProfile;

    QString     fromProfile =
        "/Users/jaapg/Development/PhotoStage/PhotoStage/ICCProfiles/" +
        from +
        ".icc";

    QString toProfile;

    if (to.at(0) == QDir::separator())
    {
        toProfile = to;
        qDebug() << "Loading monitor profile";
    }
    else
        toProfile =
            "/Users/jaapg/Development/PhotoStage/PhotoStage/ICCProfiles/" + to +
            ".icc";

    hInProfile  = cmsOpenProfileFromFile(fromProfile.toLocal8Bit().data(), "r");
    hOutProfile = cmsOpenProfileFromFile(toProfile.toLocal8Bit().data(), "r");

#define MY_TYPE (FLOAT_SH(1) | COLORSPACE_SH(PT_RGB) | CHANNELS_SH(3) | \
    BYTES_SH(4) | DOSWAP_SH(1))

    cmsUInt32Number inputFormat  = MY_TYPE;
    cmsUInt32Number outputFormat = MY_TYPE;

    if (inFormat == FORMAT_RGB32)
        inputFormat = TYPE_BGRA_8;
    else if (inFormat == FORMAT_GRAYSCALE8)
        inputFormat = (COLORSPACE_SH(PT_RGB) | CHANNELS_SH(1) | BYTES_SH(1));

    if (outFormat == FORMAT_RGB32)
        outputFormat = TYPE_BGRA_8;

    mHTransform =
        QSharedPointer<char>((char*)cmsCreateTransform(hInProfile, inputFormat,
            hOutProfile, outputFormat, INTENT_PERCEPTUAL, 0), transform_delete);

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

    Image        outImage(inImage.size());

    const float* inbuf  = inImage.data();
    float*       outbuf = outImage.data();

    cmsDoTransform((cmsHTRANSFORM)mHTransform.data(), inbuf, outbuf, inImage.width() * inImage.height());

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

        cmsDoTransform((cmsHTRANSFORM)mHTransform.data(), inLine, outLine, width);
    }
    return outImage;
}

QImage ColorTransform::transformQImage(const QImage& inImage) const
{
    int      width  = inImage.width();
    int      height = inImage.height();

    uint8_t* buffer = new uint8_t[width * height * 4];

    for (int y = 0; y < height; y++)
    {
        const uint8_t* inLine  = inImage.constScanLine(y);
        uint8_t*       outLine = &buffer[y * width * 4];

        cmsDoTransform((cmsHTRANSFORM)mHTransform.data(), inLine, outLine, width);

        // TODO: optimization.. littleCMS can convert in place
        // when source and dest image format organization are the same
        for (int x = 0; x < width; x++)
            // set alpha value to 0xff (because little CMS does not)
            outLine[x * 4 + 3] = 0xff;
    }

    return QImage(buffer, width, height, 4 * width,
               QImage::Format_RGB32, PhotoStageFreeImageBuffer, buffer);
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
        cmsDoTransform((cmsHTRANSFORM)mHTransform.data(), inLine, outLine, width);
    }
    return outImage;
}
}
