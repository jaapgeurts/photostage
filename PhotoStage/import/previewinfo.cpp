#include "previewinfo.h"

namespace PhotoStage {
PreviewInfo::PreviewInfo() : image(), filePath(), isInLibrary(false)
{
}

PreviewInfo::PreviewInfo(const QImage& img, const QString& path)
    : image(img), filePath(path), isInLibrary(false)
{
}
} // namespace PhotoStage
