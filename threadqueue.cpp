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
    qDeleteAll(mJobs);
    mJobs.clear();
}

void ThreadQueue::addJob(Runnable* runnable)
{
    // TODO: these two lines should be atomic

    mMutexJobs.lock();
    bool shouldStart = mJobs.isEmpty();
    mJobs.enqueue(runnable);
    qDebug() << "Added queue size:" << mJobs.size();
    mMutexJobs.unlock();

    if (shouldStart)
    {
        if (thread() != &mThread)
        {
            moveToThread(&mThread);
            connect(&mThread, &QThread::started,
                this, &ThreadQueue::onStarted);
            connect(this, &ThreadQueue::finished,
                &mResultForwarder, &ResultForwarder::onFinished);
            // TODO: connect errors
        }
        mThread.start();
    }
}

void ThreadQueue::onStarted()
{
    Runnable* r;

    while ((r = hasMore()) != NULL)
    {
        QVariant result = r->run();
        emit     finished(r, result);
    }
    thread()->quit();
}

Runnable* ThreadQueue::hasMore()
{
    Runnable* r = NULL;

    mMutexJobs.lock();

    if (!mJobs.isEmpty())
        r = mJobs.dequeue();
    qDebug() << "Removed queue size:" << mJobs.size();
    mMutexJobs.unlock();
    return r;
}
}
