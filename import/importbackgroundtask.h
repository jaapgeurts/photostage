#ifndef IMPORTBACKGROUNDTASK_H
#define IMPORTBACKGROUNDTASK_H

#include "backgroundtask.h"
#include "workunits/importworkunit.h"


class ImportBackgroundTask : public BackgroundTask
{
public:
    ImportBackgroundTask(const ImportInfo & info);

    int progressMinimum();
    int progressMaximum();

public slots:
    void start();
    void cancel();

private:
    QString mName;
    ImportInfo mInfo;



    bool importFile(const QFileInfo &info);
};

#endif // IMPORTBACKGROUNDTASK_H
