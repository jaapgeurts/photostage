#ifndef ACTIONSTATEGROUP_H
#define ACTIONSTATEGROUP_H

#include <QList>
#include <QAction>
#include <QObject>

class ActionStateGroup : QObject
{
    Q_OBJECT

    public:

        ActionStateGroup(QObject *parent = 0);

        void addAction(QAction* action);

    public slots:

        void enableAll(bool enabled);
        void disableAll(bool disabled);
        void enableAll();
        void disableAll();

    private:

        QList<QAction*> mListActions;
};

#endif // ACTIONSTATEGROUP_H
