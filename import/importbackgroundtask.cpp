#include "importbackgroundtask.h"

namespace PhotoStage
{
ImportBackgroundTask::ImportBackgroundTask(const ImportInfo& info) :
    BackgroundTask("Import Task")
{
    setDescription(QString("Importing..."));
    mInfo = info;

    mWorkUnit = ImportWorkUnit::instance();

    setAutoDelete(false);
}

int ImportBackgroundTask::progressMinimum()
{
    return 0;
}

int ImportBackgroundTask::progressMaximum()
{
    return mInfo.files().size() - 1;
}

void ImportBackgroundTask::run()
{
    int                      i = 0;

    QListIterator<QFileInfo> it(mInfo.files());
    mWorkUnit->beginImport();

    while (it.hasNext() && mRunning)
    {
        QFileInfo info = it.next();
        long long id   = mWorkUnit->importPhoto(info, mInfo.options());

        if (id == -1)
            qDebug() << "There was an error importing file:" <<
                info.filePath();
        else
            mIdList.append(id);
        i++;
        emit progressUpdated(i);
    }
    emit taskFinished(this);
}

void ImportBackgroundTask::start()
{
    // TODO: consider importing files in parallel, not in sequence
    mRunning = true;
    QThreadPool::globalInstance()->start(this);
}

void ImportBackgroundTask::cancel()
{
    mRunning = false;
}
}