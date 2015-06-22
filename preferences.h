#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QObject>

namespace PhotoStage
{
class Preferences : public QObject
{
    Q_OBJECT

    public:

        static Preferences* instance();

        // Library section
        bool library_include_subfolders;

    signals:

    public slots:

        void load();
        void save();

    private:

        static Preferences* mPreferences;
        explicit Preferences(QObject* parent = 0);
};
}
#endif // PREFERENCES_H