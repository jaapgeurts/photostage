#ifndef PHOTOSTAGE_IMPORTINFO_H
#define PHOTOSTAGE_IMPORTINFO_H

#include <QFileInfo>
#include <QList>
#include <QString>

#include "importoptions.h"

namespace PhotoStage {
struct ImportInfo {
public:
  ImportInfo();

  ImportInfo(const ImportInfo& src);
  explicit ImportInfo(const QList<QFileInfo>&   sourceFiles,
                      const QFileInfo&          destinationPath,
                      ImportOptions::ImportMode mode);

  QList<QFileInfo> sourceFiles;
  ImportOptions    options;
};
} // namespace PhotoStage
#endif // PHOTOSTAGE_IMPORTINFO_H
