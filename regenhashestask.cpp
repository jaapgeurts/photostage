#include <QFile>
#include <QThreadPool>
#include <QDebug>

#include "regenhashestask.h"

namespace PhotoStage
{
RegenHashesTask::RegenHashesTask(const QList<Photo>& list) :
    BackgroundTask("Import Task"),
    mRunning(false),
    mPhotoList(list)
{
    mPhotoWorkUnit = PhotoWorkUnit::instance();
    setDescription(QString("Calculating hashes..."));
    setAutoDelete(true);
    mPhotoWorkUnit = PhotoWorkUnit::instance();
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
    int                  i = 0;

    QListIterator<Photo> it(mPhotoList);

    mPhotoWorkUnit->beginTransaction();

    while (it.hasNext() && mRunning)
    {
        Photo p = it.next();
        mPhotoWorkUnit->regenerateHash(p);
        i++;
        emit progressUpdated(i);
    }
    mPhotoWorkUnit->endTransaction();
    emit taskFinished(this);
}

void RegenHashesTask::start()
{
    // TODO: consider importing files in parallel, not in sequence
    mRunning = true;
    QThreadPool::globalInstance()->start(this);
}

void RegenHashesTask::cancel()
{
    mRunning = false;
}
}
