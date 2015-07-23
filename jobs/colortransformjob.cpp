#include <QDebug>

#include "engine/colortransform.h"
#include "colortransformjob.h"

namespace PhotoStage
{
ColorTransformJob::ColorTransformJob(const Photo& photo) :
    QObject(NULL),
    mPhoto(photo)
{
    setName("ColorTransformJob");
}

QVariant ColorTransformJob::run()
{
    ColorTransform transform = ColorTransform::getTransform(
        "Melissa-sRGB-RGB32",
        WORKING_COLOR_SPACE,
        ColorTransform::getMonitorProfilePath(),
        ColorTransform::FORMAT_RGB32,
        ColorTransform::FORMAT_RGB32);
    QImage translated = transform.transformQImage(mPhoto.libraryPreview());

    return translated;
}

void ColorTransformJob::finished(QVariant result)
{
    QImage image = result.value<QImage>();
    emit   imageReady(mPhoto, image);
}

void ColorTransformJob::error(const QString& error)
{
}

void ColorTransformJob::cancel()
{
    mPhoto.setIsDownloading(false);
}
}
