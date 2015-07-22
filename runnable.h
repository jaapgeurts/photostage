#ifndef PHOTOSTAGE_RUNNABLE
#define PHOTOSTAGE_RUNNABLE

#include <QVariant>

namespace PhotoStage
{
class Runnable
{
    public:

        explicit Runnable();
        virtual ~Runnable();

        virtual QVariant run()                   = 0;
        virtual void finished(QVariant result)   = 0;
        virtual void error(const QString& error) = 0;
        virtual void cancel();

        bool autoDelete() const;
        void setAutoDelete(bool enable);

        void setName(const QString& name);
        const QString& name() const;

    private:

        bool    mAutoDelete;
        QString mName;
};
}
#endif // PHOTOSTAGE_RUNNABLE
