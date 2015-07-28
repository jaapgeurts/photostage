#include "previewinfo.h"

namespace PhotoStage
{
PreviewInfo::PreviewInfo() :
    filePath(),
    image(),
    isInLibrary(false)
{
}

PreviewInfo::PreviewInfo(const QImage& img, const QString& path) :
    filePath(path),
    image(img),
    isInLibrary(false)
{
}
}
