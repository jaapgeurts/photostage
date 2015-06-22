#include "importinfo.h"

namespace PhotoStage
{
ImportInfo::ImportInfo(const ImportInfo& src)
{
    mSourceFiles = src.mSourceFiles;
    mOptions     = src.mOptions;
}

ImportInfo::ImportInfo(const QList<QFileInfo>& sourceFiles,
    const QFileInfo& destinationDir,
    ImportOptions::ImportMode mode)
{
    mSourceFiles = sourceFiles;
    mOptions     = ImportOptions(destinationDir, mode);
}

const QList<QFileInfo>& ImportInfo::files() const
{
    return mSourceFiles;
}

const ImportOptions ImportInfo::options() const
{
    return mOptions;
}
}