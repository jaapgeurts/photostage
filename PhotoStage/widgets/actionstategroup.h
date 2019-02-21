#ifndef ACTIONSTATEGROUP_H
#define ACTIONSTATEGROUP_H

#include <QAction>
#include <QList>
#include <QObject>

namespace PhotoStage {
class ActionStateGroup : QObject
{
  Q_OBJECT

public:
  ActionStateGroup(QObject* parent = 0);

  void addAction(QAction* action);

public slots:

  void enableAll(bool enabled);
  void disableAll(bool disabled);
  void enableAll();
  void disableAll();

private:
  QList<QAction*> mListActions;
};
} // namespace PhotoStage
#endif // ACTIONSTATEGROUP_H
