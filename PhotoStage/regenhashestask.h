#ifndef PHOTOSTAGE_REGENHASHESTASK_H
#define PHOTOSTAGE_REGENHASHESTASK_H

#include <QList>

#include "backgroundtask.h"
#include "database/databaseaccess.h"

namespace PhotoStage {
class RegenHashesTask : public BackgroundTask, public QRunnable
{
  Q_OBJECT

public:
  RegenHashesTask(const QList<Photo>& list);

  // BackgroundTask interface

public:
  int  progressMinimum();
  int  progressMaximum();
  void run();

public slots:

  void start();
  void cancel();

private:
  std::atomic<bool> mRunning;
  PhotoDAO*         mPhotoWorkUnit;
  QList<Photo>      mPhotoList;
};
} // namespace PhotoStage
#endif // PHOTOSTAGE_REGENHASHESTASK_H
