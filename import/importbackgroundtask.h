#ifndef IMPORTBACKGROUNDTASK_H
#define IMPORTBACKGROUNDTASK_H

#include <QRunnable>
#include <QList>

#include "backgroundtask.h"
#include "database/importdao.h"

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

        QList<long long>  mIdList;
        ImportDAO*   mWorkUnit;
        std::atomic<bool> mRunning;
};
}

#endif // IMPORTBACKGROUNDTASK_H
