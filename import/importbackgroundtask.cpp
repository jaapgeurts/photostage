#include "importbackgroundtask.h"

namespace PhotoStage
{
ImportBackgroundTask::ImportBackgroundTask(const ImportInfo& info) :
    BackgroundTask("Import Task"),
    mInfo(info)

{
    setDescription(QString("Importing..."));

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
    DatabaseAccess::photoDao()->beginImport();

    // create the new Import Collection
    long long importcollectionid = DatabaseAccess::collectionDao()->addImportCollection(mInfo.files().size());

    while (it.hasNext() && mRunning)
    {
        QFileInfo info = it.next();
        DatabaseAccess::photoDao()->importPhoto(importcollectionid, info, mInfo.options());
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
