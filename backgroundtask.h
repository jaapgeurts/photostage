#ifndef BACKGROUNDTASK_H
#define BACKGROUNDTASK_H

#include <QObject>
#include <QRunnable>

namespace PhotoStage
{
    class BackgroundTask : public QObject
    {
        Q_OBJECT

        public:

            BackgroundTask(const QString& name);

            const QString taskName() const
            {
                return mName;
            }

            void setDescription(const QString& description);

            const QString description() const
            {
                return mDescription;
            }

            virtual int progressMinimum() = 0;
            virtual int progressMaximum() = 0;

        public slots:

            virtual void start()  = 0;
            virtual void cancel() = 0;

        signals:

            void taskFinished(BackgroundTask* task);
            void taskCancelled(BackgroundTask* task);
            void progressUpdated(int value);

        private:

            QString mName;
            QString mDescription;
    };
}

#endif // BACKGROUNDTASK_H