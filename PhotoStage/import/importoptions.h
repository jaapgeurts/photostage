#ifndef PHOTOSTAGE_IMPORTOPTIONS_H
#define PHOTOSTAGE_IMPORTOPTIONS_H

#include <QFileInfo>

namespace PhotoStage {
struct ImportOptions {
public:
  /* ImportMove: Move files to target,
   * ImportCopy: Copy files to target leaving source intact,
   * ImportAdd: Do nothing just add the references into the catalog.
   */
  enum ImportMode { ImportMove = 0, ImportCopy = 1, ImportAdd = 2 };
  ImportOptions();
  ImportOptions(const QFileInfo& destDir, const ImportMode& importMode);

  QFileInfo  destinationDir;
  ImportMode importMode;
  // Preserve the structure when importing and moving/copying of the source
  // folder or copy all files into the selected target folder
  bool preserveDirectoryStructure;
};
} // namespace PhotoStage
#endif // PHOTOSTAGE_IMPORTOPTIONS_H
