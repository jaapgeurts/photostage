#include "importinfo.h"

ImportInfo::ImportInfo(const QList<QFileInfo> &sourceFiles, const QFileInfo & destinationDir, ImportMode mode)
{
    mSourceFiles = sourceFiles;
    mDestinationDir = destinationDir;
    mImportMode = mode;
}

const QList<QFileInfo>& ImportInfo::getFiles() const
{
  return mSourceFiles;
}

const QFileInfo & ImportInfo::getDestinationDir() const
{
    return mDestinationDir;
}

ImportInfo::ImportMode ImportInfo::getImportMode() const
{
    return mImportMode;
}


