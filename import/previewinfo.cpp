#include "previewinfo.h"

namespace PhotoStage
{
PreviewInfo::PreviewInfo(const QImage& img, const QString& path)
{
    this->filePath = path;
    image          = img;
}
}