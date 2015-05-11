#include "importinfo.h"

ImportInfo::ImportInfo(const QList<QFileInfo> &sourceFiles, const QFileInfo & destinationDir)
{
    mSourceFiles = sourceFiles;
    mDestinationDir = destinationDir;
}

const QList<QFileInfo>& ImportInfo::getFiles() const
{
  return mSourceFiles;
}

const QFileInfo & ImportInfo::getDestinationDir() const
{
    return mDestinationDir;
}


