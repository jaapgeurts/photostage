#include "importinfo.h"

ImportInfo::ImportInfo(const QList<QFileInfo> &sourceFiles, const QFileInfo & destinationDir, ImportMode mode)
{
    mSourceFiles = sourceFiles;
    mDestinationDir = destinationDir;
    mImportMode = mode;
}

const QList<QFileInfo>& ImportInfo::files() const
{
  return mSourceFiles;
}

const QFileInfo & ImportInfo::destinationDir() const
{
    return mDestinationDir;
}

ImportInfo::ImportMode ImportInfo::importMode() const
{
    return mImportMode;
}


