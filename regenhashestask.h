#ifndef PHOTOSTAGE_REGENHASHESTASK_H
#define PHOTOSTAGE_REGENHASHESTASK_H

#include <QList>

#include "database/databaseaccess.h"
#include "backgroundtask.h"

namespace PhotoStage
{
class RegenHashesTask : public BackgroundTask, public QRunnable
{
    Q_OBJECT

    public:

        RegenHashesTask(const QList<Photo>& list);

        // BackgroundTask interface

    public:

        int progressMinimum();
        int progressMaximum();
        void run();

    public slots:

        void start();
        void cancel();

    private:

        std::atomic<bool> mRunning;
        PhotoDAO*         mPhotoWorkUnit;
        QList<Photo>      mPhotoList;
};
}
#endif // PHOTOSTAGE_REGENHASHESTASK_H
