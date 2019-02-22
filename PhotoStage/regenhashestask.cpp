#include <QDebug>
#include <QFile>
#include <QThreadPool>

#include "regenhashestask.h"

namespace PhotoStage {
RegenHashesTask::RegenHashesTask(const QList<Photo>& list)
    : BackgroundTask("Import Task"), mRunning(false), mPhotoList(list)
{
  mPhotoWorkUnit = DatabaseAccess::photoDao();
  setDescription(QString("Calculating hashes..."));
  setAutoDelete(true);
}

int RegenHashesTask::progressMinimum()
{
  return 0;
}

int RegenHashesTask::progressMaximum()
{
  return mPhotoList.size() - 1;
}

void RegenHashesTask::run()
{
  int i = 0;

  QListIterator<Photo> it(mPhotoList);

  DatabaseAccess::instance()->beginTransaction();

  while (it.hasNext() && mRunning)
  {
    Photo p = it.next();
    mPhotoWorkUnit->regenerateHash(p);
    i++;
    emit progressUpdated(i);
  }
  DatabaseAccess::instance()->endTransaction();
  emit taskFinished(this);
}


} // namespace PhotoStage
