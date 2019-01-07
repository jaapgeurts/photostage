#include <QDebug>

#include "engine/colortransform.h"
#include "colortransformjob.h"

namespace PhotoStage
{
ColorTransformJob::ColorTransformJob(const Photo& photo, ConversionType type) :
    QObject(NULL),
    mPhoto(photo),
    mType(type)
{
    setName("ColorTransformJob");
}

QVariant ColorTransformJob::run()
{
    QString srcProfile, dstProfile;
    QImage  translated;

    if (mType == Preview)
    {
        srcProfile = PREVIEW_COLOR_SPACE;
        dstProfile = ColorTransform::getMonitorProfilePath();
        QString        iden      = srcProfile + dstProfile;
        ColorTransform transform = ColorTransform::getTransform(iden, srcProfile, dstProfile,
                ColorTransform::FORMAT_RGB32, ColorTransform::FORMAT_RGB32);
        translated = transform.transformQImage(mPhoto.libraryPreview());
    }
    else if (mType == Develop)
    {
        srcProfile = WORKING_COLOR_SPACE;
        dstProfile = ColorTransform::getMonitorProfilePath();
        QString        iden      = srcProfile + dstProfile;
        ColorTransform transform = ColorTransform::getTransform(iden, srcProfile, dstProfile,
                ColorTransform::FORMAT_BGR48_PLANAR, ColorTransform::FORMAT_RGB32);
        translated = transform.transformToQImage(mPhoto.originalImage());
    }

    return translated;
}

void ColorTransformJob::finished(QVariant result)
{
    QImage image = result.value<QImage>();
    emit   imageReady(mPhoto, image);
}

void ColorTransformJob::error(const QString& /*error*/)
{
}

void ColorTransformJob::cancel()
{
    mPhoto.setIsDownloadingPreview(false);
}
}
