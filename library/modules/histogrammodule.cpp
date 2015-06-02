#include <QVBoxLayout>
#include <QDebug>

#include <lcms2.h>

#include "histogrammodule.h"

HistogramModule::HistogramModule(QWidget *parent) : LibraryModule(parent)
{
    mHistogram = new Histogram(this);
    setLayout(new QVBoxLayout(this));
    layout()->setContentsMargins(0,0,0,0);
    layout()->addWidget(mHistogram);
}

void HistogramModule::setPhotos(const QList<Photo *> &list)
{
    LibraryModule::setPhotos(list);
    if (list.size() == 1)
    {
        Photo * photo = list.at(0);

        PhotoData image = loadImage(QImage(photo->rawImage()));
        mHistogram->setImageData(image);
    }
}

PhotoData HistogramModule::loadImage(const QImage& inImage)
{
    cmsHPROFILE hInProfile, hOutProfile;
    cmsHTRANSFORM hTransform;

    hInProfile = cmsOpenProfileFromFile("/Users/jaapg/Development/PhotoStage/PhotoStage/ICCProfiles/sRGB.icc","r");
    hOutProfile = cmsOpenProfileFromFile("/Users/jaapg/Development/PhotoStage/PhotoStage/ICCProfiles/MelissaRGB.icc","r");

    hTransform = cmsCreateTransform(hInProfile,TYPE_BGRA_8,hOutProfile,TYPE_RGB_16,INTENT_PERCEPTUAL,0);

    cmsCloseProfile(hInProfile);
    cmsCloseProfile(hOutProfile);

    PhotoData result = PhotoData(inImage.size());

    qDebug() << "Has alpha" << (inImage.hasAlphaChannel() ? "yes" : "no");
    if (inImage.format() != QImage::Format_RGB32) {
        qDebug() << "Image not 32 bit:" <<inImage.format();

    }

    for (int i=0;i<inImage.height();i++)
    {
        const uchar *inbuf = inImage.constScanLine(i);
        uint16_t *outbuf = result.scanLine(i);
        //    const uchar *inbuf = image.constBits();
        //    uint16_t *outbuf = result.data();
        //    if (outbuf == NULL)
        //        qDebug() << "outbuf == NULL";
        cmsDoTransform(hTransform,inbuf,outbuf,inImage.width() );//* image.height());
    }
    cmsDeleteTransform(hTransform);
    return result;
}





