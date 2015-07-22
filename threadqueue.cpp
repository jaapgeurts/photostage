#include <QDebug>

#include "threadqueue.h"

namespace PhotoStage
{
void ResultForwarder::onFinished(Runnable* runnable, const QVariant& result)
{
    runnable->finished(result);

    if (runnable->autoDelete())
        delete runnable;
}

ThreadQueue::ThreadQueue() :
    QObject(NULL),
    mThread(this)
{
    //qDebug() << "worker thread " << mModelIndex.row() <<" created";
}

ThreadQueue::~ThreadQueue()
{
    //qDebug() << "worker thread " << mModelIndex.row() <<" deleted";
    cancel();
}

void ThreadQueue::addJob(Runnable* runnable)
{
    // TODO: these two lines should be atomic

    mMutexJobs.lock();
    mJobs.enqueue(runnable);
    //qDebug() << "Added queue size:" << mJobs.size();
    mMutexJobs.unlock();

    // Make sure our object(ThreadQueue)
    // is running in its own thread.
    bool hasMoved = thread() == &mThread;
    //qDebug() << "Queue has moved" << (hasMoved ? "yes" : "no");

    if (!hasMoved)
    {
        moveToThread(&mThread);

        connect(&mThread, &QThread::started,
            this, &ThreadQueue::onStarted);
        connect(this, &ThreadQueue::finished,
            &mResultForwarder, &ResultForwarder::onFinished);
    }
    // TODO: connect errors

    //qDebug() << "Starting thread";
    // calling run twice is not a problem
    mThread.start();
}

void ThreadQueue::onStarted()
{
    QThread* t = thread();

    if (t->isFinished())
    {
        qDebug() << "Not running because finished";
        return;
    }

    if (t->isInterruptionRequested())
    {
        qDebug() << "Not running because interrupt requested";
        return;
    }

    Runnable* r;
    // qDebug() << "has started";

    while ((r = hasMore()) != NULL)
    {
        qDebug() << "executing next from list" << r->name();
        QVariant result = r->run();
        emit     finished(r, result);
    }
    t->quit();
}

void ThreadQueue::cancel()
{
    //qDebug() << "Clearing jobs";
    mMutexJobs.lock();
    foreach(Runnable * r, mJobs)
    r->cancel();
    qDeleteAll(mJobs);
    mJobs.clear();
    mMutexJobs.unlock();
}

Runnable* ThreadQueue::hasMore()
{
    Runnable* r = NULL;

    mMutexJobs.lock();

    if (!mJobs.isEmpty())
        r = mJobs.dequeue();
    //  qDebug() << "Removed queue size:" << mJobs.size();
    mMutexJobs.unlock();
    return r;
}
}
