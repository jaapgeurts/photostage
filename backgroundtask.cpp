#include "backgroundtask.h"

BackgroundTask::BackgroundTask(const QString & name)
{
    mName = name;
}

void BackgroundTask::setDescription(const QString &description)
{
    mDescription = description;
}