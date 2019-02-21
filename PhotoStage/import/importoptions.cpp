#include "importoptions.h"

namespace PhotoStage {
ImportOptions::ImportOptions()
{
}

ImportOptions::ImportOptions(const QFileInfo& destDir, const ImportMode& mode)
    : destinationDir(destDir), importMode(mode)
{
}
} // namespace PhotoStage
