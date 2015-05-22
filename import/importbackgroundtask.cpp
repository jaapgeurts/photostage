
#include <QtConcurrent>

#include "importbackgroundtask.h"

ImportBackgroundTask::ImportBackgroundTask(const ImportInfo & info) : BackgroundTask("Import Task")
{
    setDescription(QString("Importing..."));
    mInfo = info;



}

int ImportBackgroundTask::progressMinimum()
{
    return 0;
}

int ImportBackgroundTask::progressMaximum()
{
    return mInfo.files().size()-1;
}

void reduce(bool & result, const bool & intermediate)
{
    if (result)
        result = intermediate;
}

struct Importer
{
    Importer(const ImportOptions& options) : mOptions(options)
    {
        mWorkUnit = ImportWorkUnit::instance();
    }

    typedef bool result_type;

    bool operator()(const QFileInfo& info)
    {
        return mWorkUnit->importPhoto(info,mOptions);
    }

    ImportOptions mOptions;
    ImportWorkUnit *mWorkUnit;
};

void ImportBackgroundTask::start()
{
    // TODO: consider importing files in parallel, not in sequence
    QFuture<bool> result = QtConcurrent::mappedReduced(mInfo.files(),Importer(mInfo.options()),&reduce);
}

bool ImportBackgroundTask::importFile(const QFileInfo & info)
{

}

void ImportBackgroundTask::cancel()
{

}

