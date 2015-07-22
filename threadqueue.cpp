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
    mThread(this),
    mLastId(1)
{
    //qDebug() << "worker thread " << mModelIndex.row() <<" created";
}

ThreadQueue::~ThreadQueue()
{
    //qDebug() << "worker thread " << mModelIndex.row() <<" deleted";
    purgeKeep();
}

uint32_t ThreadQueue::addJob(Runnable* runnable)
{
    mMutexJobs.lock();
    mJobs.enqueue(runnable);
    runnable->setId(++mLastId);
    //qDebug() << "Added queue size:" << mJobs.size();
    mMutexJobs.unlock();

    bool hasMoved = thread() == &mThread;
    //qDebug() << "Queue has moved" << (hasMoved ? "yes" : "no");

    if (!hasMoved)
    {
        // Make sure our object(ThreadQueue)
        // is running in its own thread.
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
    return mLastId;
}

void ThreadQueue::onStarted()
{
    Runnable* r;

    // qDebug() << "has started";

    while ((r = hasMore()) != NULL)
    {
        qDebug() << "executing next from list" << r->name();
        QVariant result = r->run();
        emit     finished(r, result);
    }
    thread()->quit();
}

void ThreadQueue::purgeKeep()
{
    //qDebug() << "Clearing jobs";
    mMutexJobs.lock();
    foreach(Runnable * r, mJobs)
    r->cancel();
    qDeleteAll(mJobs);
    mJobs.clear();
    mMutexJobs.unlock();
}

void ThreadQueue::purgeKeep(const QList<uint32_t>& list)
{
    if (list.isEmpty())
        return;

    mMutexJobs.lock();
    qDebug() << "Searching for jobs to cancel";
    foreach(Runnable * r, mJobs)
    {
        if (!list.contains(r->id()))
        {
            qDebug() << "Cancelling job" << r->id();
            r->cancel();
            mJobs.removeAll(r);
            delete r;
        }
    }
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
