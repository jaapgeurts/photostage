#ifndef PHOTOSTAGE_IMPORTBACKGROUNDTASK_H
#define PHOTOSTAGE_IMPORTBACKGROUNDTASK_H

#include <QList>
#include <QRunnable>

#include "backgroundtask.h"
#include "database/databaseaccess.h"
#include "importinfo.h"
#include "importoptions.h"

namespace PhotoStage {
class ImportBackgroundTask : public BackgroundTask, public QRunnable
{
public:
  ImportBackgroundTask(const ImportInfo& info);

  int  progressMinimum();
  int  progressMaximum();
  void run();

  const QList<long long>& resultList();

public slots:

  void start();
  void cancel();

private:
  QString   mLastPath;
  long long mLastPathId;

  QString    mName;
  ImportInfo mInfo;

  std::atomic<bool> mRunning;

  ExifInfo preparePhotoImport(const QFileInfo&     fileInfo,
                              const ImportOptions& options);
};
} // namespace PhotoStage

#endif // IMPORTBACKGROUNDTASK_H
