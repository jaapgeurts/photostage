

#include "importbackgroundtask.h"

ImportBackgroundTask::ImportBackgroundTask(const ImportInfo & info) : BackgroundTask("Import Task")
{
    setDescription(QString("Importing..."));
    mInfo = info;

    mWorkUnit = ImportWorkUnit::instance();

}

int ImportBackgroundTask::progressMinimum()
{
    return 0;
}

int ImportBackgroundTask::progressMaximum()
{
    return mInfo.files().size()-1;
}

void ImportBackgroundTask::run()
{
    int i =0;
    QListIterator<QFileInfo> it(mInfo.files());
    while (it.hasNext() && running)
    {
        QFileInfo info = it.next();
        if (!mWorkUnit->importPhoto(info,mInfo.options()))
            qDebug() << "There was an error importing file:"<<info.filePath();
        i++;
        emit progressUpdated(i);
    }
    emit taskFinished(this);
}


void ImportBackgroundTask::start()
{
    // TODO: consider importing files in parallel, not in sequence
    running = true;
    QThreadPool::globalInstance()->start(this);
}


void ImportBackgroundTask::cancel()
{
    running = false;
}

