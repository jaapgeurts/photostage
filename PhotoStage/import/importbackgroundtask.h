#ifndef PHOTOSTAGE_IMPORTBACKGROUNDTASK_H
#define PHOTOSTAGE_IMPORTBACKGROUNDTASK_H

#include <QList>

#include "backgroundtask.h"
#include "database/databaseaccess.h"
#include "importinfo.h"
#include "importoptions.h"

namespace PhotoStage {
class ImportBackgroundTask : public BackgroundTask
{
public:
  ImportBackgroundTask(const ImportInfo& info);

  // inherit from QRunnable
  int  progressMinimum();
  int  progressMaximum();
  void run();

  const QList<long long>& resultList();

private:
  QString   mLastPath;
  long long mLastPathId;

  QString    mName;
  ImportInfo mInfo;

  ExifInfo preparePhotoImport(const QFileInfo&     fileInfo,
                              const ImportOptions& options);
};
} // namespace PhotoStage

#endif // IMPORTBACKGROUNDTASK_H
