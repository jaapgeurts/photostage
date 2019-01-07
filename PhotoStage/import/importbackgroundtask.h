#ifndef PHOTOSTAGE_IMPORTBACKGROUNDTASK_H
#define PHOTOSTAGE_IMPORTBACKGROUNDTASK_H

#include <QRunnable>
#include <QList>

#include "backgroundtask.h"
#include "database/databaseaccess.h"

namespace PhotoStage
{
class ImportBackgroundTask : public BackgroundTask, public QRunnable
{
    public:

        ImportBackgroundTask(const ImportInfo& info);

        int progressMinimum();
        int progressMaximum();
        void run();

        const QList<long long>& resultList();

    public slots:

        void start();
        void cancel();

    private:

        QString           mName;
        ImportInfo        mInfo;

        std::atomic<bool> mRunning;
};
}

#endif // IMPORTBACKGROUNDTASK_H
