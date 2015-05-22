#ifndef IMPORTBACKGROUNDTASK_H
#define IMPORTBACKGROUNDTASK_H

#include <QRunnable>

#include "backgroundtask.h"
#include "workunits/importworkunit.h"


class ImportBackgroundTask : public BackgroundTask, public QRunnable
{
public:
    ImportBackgroundTask(const ImportInfo & info);

    int progressMinimum();
    int progressMaximum();
    void run();

public slots:
    void start();
    void cancel();

private:
    QString mName;
    ImportInfo mInfo;


    ImportWorkUnit * mWorkUnit;
    std::atomic<bool> running;
};

#endif // IMPORTBACKGROUNDTASK_H
