#ifndef PHOTOSTAGE_THREADQUEUE_H
#define PHOTOSTAGE_THREADQUEUE_H

#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QThread>

#include "runnable.h"

namespace PhotoStage {
class ResultForwarder : public QObject
{
  Q_OBJECT

public slots:

  void onFinished(Runnable* runnable, const QVariant& result);
};

class ThreadQueue : public QObject
{
  Q_OBJECT

public:
  explicit ThreadQueue();
  virtual ~ThreadQueue();

  uint32_t addJob(Runnable* runnable);

  void purgeExcept(const QList<uint32_t>& list);

signals:

  void finished(Runnable* runnable, const QVariant& result);

  // void error(int code, QString error);

public slots:

  void onStarted();
  void cancel();

private:
  QMutex            mMutexJobs;
  QQueue<Runnable*> mJobs;
  QThread           mThread;
  ResultForwarder   mResultForwarder;
  uint32_t          mLastId;

  Runnable* hasMore();
};
} // namespace PhotoStage
#endif // PHOTOSTAGE_THREADQUEUE_H
