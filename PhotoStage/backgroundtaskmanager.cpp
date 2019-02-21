#include <QLayout>

#include "backgroundtaskmanager.h"

namespace PhotoStage {
BackgroundTaskManager::BackgroundTaskManager(QWidget* const container,
                                             QObject*       parent)
    : QObject(parent)
{
  mContainer = container;
}

BackgroundTaskManager::~BackgroundTaskManager()
{
}

void BackgroundTaskManager::addRunnable(BackgroundTask* const task)
{
  BackgroundTaskProgress* progress = new BackgroundTaskProgress(mContainer);

  progress->setProgressRange(task->progressMinimum(), task->progressMaximum());
  progress->setTaskName(task->description());

  // connect task signals to the interface
  connect(task, &BackgroundTask::taskFinished, this,
          &BackgroundTaskManager::taskFinished);
  connect(task, &BackgroundTask::progressUpdated, progress,
          &BackgroundTaskProgress::updateProgress);

  // connection interface signal to the task
  connect(progress, &BackgroundTaskProgress::cancelClicked, task,
          &BackgroundTask::cancel);
  mDict.insert(task, progress);
  mContainer->layout()->addWidget(progress);
}

bool BackgroundTaskManager::isWorking()
{
  return mDict.size() > 0;
}

void BackgroundTaskManager::taskFinished(BackgroundTask* task)
{
  BackgroundTaskProgress* progress = mDict.value(task);

  mContainer->layout()->removeWidget(progress);
  mDict.remove(task);

  progress->deleteLater();
}

void BackgroundTaskManager::cancelAll()
{
  foreach (BackgroundTask* task, mDict.keys())
  {
    task->cancel();
  }
}
} // namespace PhotoStage
