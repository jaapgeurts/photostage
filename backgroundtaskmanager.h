#ifndef BACKGROUNDTASKMANAGER_H
#define BACKGROUNDTASKMANAGER_H
#include <QObject>
#include "widgets/backgroundtaskprogress.h"
#include "backgroundtask.h"

namespace PhotoStage
{
    class BackgroundTaskManager : public QObject
    {
        Q_OBJECT

        public:

            explicit BackgroundTaskManager(QWidget* const container,
                QObject* parent = 0);
            ~BackgroundTaskManager();
            void addRunnable(BackgroundTask* const task);

        signals:

        public slots:

            void taskFinished(BackgroundTask* task);

        private:

            QWidget*                                        mContainer;
            QHash<BackgroundTask*, BackgroundTaskProgress*> mDict;
    };
}
#endif // BACKGROUNDTASKMANAGER_H