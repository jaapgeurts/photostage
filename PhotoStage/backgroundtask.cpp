#include <QDebug>
#include <QThreadPool>
#include "backgroundtask.h"

namespace PhotoStage {
BackgroundTask::BackgroundTask(const QString& name)
{
  mName = name;
}

void BackgroundTask::setDescription(const QString& description)
{
  mDescription = description;
}

void BackgroundTask::start()
{
  // TODO: consider importing files in parallel, not in sequence
  mRunning = true;
  qDebug() << "Importer threadId before:" << this->thread()->currentThreadId();
  QThreadPool::globalInstance()->start(this);
  qDebug() << "Importer threadId after:" << this->thread()->currentThreadId();
}

void BackgroundTask::cancel()
{
  mRunning = false;
}

const QString BackgroundTask::taskName() const
{
  return mName;
}

const bool BackgroundTask::running() const
{
  return mRunning;
}

const QString BackgroundTask::description() const
{
  return mDescription;
}

} // namespace PhotoStage
