#include "importinfo.h"

namespace PhotoStage {
ImportInfo::ImportInfo(const ImportInfo& src)
    : sourceFiles(src.sourceFiles), options(src.options)
{
}

ImportInfo::ImportInfo(const QList<QFileInfo>&   sourceFiles,
                       const QFileInfo&          destinationDir,
                       ImportOptions::ImportMode mode)
    : sourceFiles(sourceFiles)
{
  options = ImportOptions(destinationDir, mode);
}
} // namespace PhotoStage
