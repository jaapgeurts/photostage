#include "actionstategroup.h"

namespace PhotoStage {
ActionStateGroup::ActionStateGroup(QObject* parent) : QObject(parent)
{
}

void ActionStateGroup::addAction(QAction* action)
{
  mListActions.append(action);
}

void ActionStateGroup::enableAll(bool enabled)
{
  foreach (QAction* action, mListActions)
  {
    action->setEnabled(enabled);
  }
}

void ActionStateGroup::disableAll(bool disabled)
{
  foreach (QAction* action, mListActions)
  {
    action->setDisabled(disabled);
  }
}

void ActionStateGroup::enableAll()
{
  enableAll(true);
}

void ActionStateGroup::disableAll()
{
  disableAll(true);
}
} // namespace PhotoStage
