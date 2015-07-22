#ifndef PHOTOSTAGE_THREADQUEUE_H
#define PHOTOSTAGE_THREADQUEUE_H

#include <QObject>
#include <QQueue>
#include <QThread>
#include <QMutex>

#include "runnable.h"

namespace PhotoStage
{
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

        void addJob(Runnable* runnable);

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

        Runnable* hasMore();
};
}
#endif // PHOTOSTAGE_THREADQUEUE_H
