#include <QVBoxLayout>

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

        QImage image = loadImage(QImage(photo->fileName));
        mHistogram->setImageData(image);
    }
}

QImage HistogramModule::loadImage(const QImage&  image)
{
    cmsHPROFILE hInProfile, hOutProfile;
    cmsHTRANSFORM hTransform;

    hInProfile = cmsOpenProfileFromFile("/Users/jaapg/Development/PhotoStage/PhotoStage/ICCProfiles/sRGB.icc","r");
    hOutProfile = cmsOpenProfileFromFile("/Users/jaapg/Development/PhotoStage/PhotoStage/ICCProfiles/MelissaRGB.icc","r");

    hTransform = cmsCreateTransform(hInProfile,TYPE_RGBA_8,hOutProfile,TYPE_RGBA_8,INTENT_PERCEPTUAL,0);

    cmsCloseProfile(hInProfile);
    cmsCloseProfile(hOutProfile);

    QImage result = QImage(image.size(),image.format());

    for (int i=0;i<image.height();i++)
    {
        const uchar *inbuf = image.constScanLine(i);
        uchar *outbuf = result.scanLine(i);
        cmsDoTransform(hTransform,inbuf,outbuf,image.width());
    }
    cmsDeleteTransform(hTransform);
    return result;
}


