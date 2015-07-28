#include "importoptions.h"

namespace PhotoStage
{
ImportOptions::ImportOptions(const QFileInfo& destDir,
    const ImportMode& mode)
{
    mDestinationDir = destDir;
    mImportMode     = mode;
}

const QFileInfo& ImportOptions::destinationDir() const
{
    return mDestinationDir;
}

const ImportOptions::ImportMode& ImportOptions::importMode() const
{
    return mImportMode;
}
}