#ifndef BACKGROUNDTASK_H
#define BACKGROUNDTASK_H

#include <QObject>
#include <QRunnable>

namespace PhotoStage {
class BackgroundTask : public QObject, protected QRunnable
{
  Q_OBJECT

public:
  BackgroundTask(const QString& name);

  const QString taskName() const;
  void setDescription(const QString& description);

  const QString description() const;
  const bool running() const;

  virtual int progressMinimum() = 0;
  virtual int progressMaximum() = 0;

  virtual void run() = 0;

public slots:

  void start();
  void cancel();

signals:

  void taskFinished(BackgroundTask* task);
  void taskCancelled(BackgroundTask* task);
  void progressUpdated(int value);

private:
  QString           mName;
  QString           mDescription;
  std::atomic<bool> mRunning;
};
} // namespace PhotoStage

#endif // BACKGROUNDTASK_H
