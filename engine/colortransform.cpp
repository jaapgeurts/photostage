#include <QDebug>

#include "colortransform.h"

ColorTransform::ColorTransform() :
    mHTransform(NULL)
{
}

ColorTransform::ColorTransform(const QString &from, const QString &to)
{
    cmsHPROFILE hInProfile, hOutProfile;

    QString     fromProfile = "/Users/jaapg/Development/PhotoStage/PhotoStage/ICCProfiles/" + from + ".icc";
    QString     toProfile   = "/Users/jaapg/Development/PhotoStage/PhotoStage/ICCProfiles/" + to + ".icc";

    hInProfile  = cmsOpenProfileFromFile(fromProfile.toLocal8Bit().data(),"r");
    hOutProfile = cmsOpenProfileFromFile(toProfile.toLocal8Bit().data(),"r");

    mHTransform = cmsCreateTransform(hInProfile,TYPE_BGRA_8,hOutProfile,TYPE_BGRA_8,INTENT_PERCEPTUAL,0);

    cmsCloseProfile(hInProfile);
    cmsCloseProfile(hOutProfile);
}

ColorTransform::~ColorTransform()
{
    cmsDeleteTransform(mHTransform);
}

bool ColorTransform::isValid() const
{
    return mHTransform != NULL;
}

QImage ColorTransform::transformImage(const QImage& inImage) const
{
    //PhotoData result = PhotoData(inImage.size());

    qDebug() << "Has alpha" << (inImage.hasAlphaChannel() ? "yes" : "no");

    if (inImage.format() != QImage::Format_RGB32)
    {
        qDebug() << "Image not 32 bit:" << inImage.format();
    }

    QImage outImage(inImage.size(),inImage.format());

    for (int i = 0; i < inImage.height(); i++)
    {
        const uchar* inbuf  = inImage.constScanLine(i);
        uchar*       outbuf = outImage.scanLine(i);
        //    const uchar *inbuf = image.constBits();
        //    uint16_t *outbuf = result.data();
        //    if (outbuf == NULL)
        //        qDebug() << "outbuf == NULL";
        cmsDoTransform(mHTransform,inbuf,outbuf,inImage.bytesPerLine() / 4);//* image.height());
    }

    return outImage;
}
