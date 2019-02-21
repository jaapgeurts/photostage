#include <QSettings>

#include "preferences.h"

#define LIBRARY_INCLUDE_SUBFOLDERS "library/include_subfolders"

namespace PhotoStage {
Preferences* Preferences::mPreferences = NULL;

Preferences* Preferences::instance()
{
  if (mPreferences == NULL)
    mPreferences = new Preferences(NULL);
  return mPreferences;
}

Preferences::Preferences(QObject* parent)
    : QObject(parent), library_include_subfolders(true)
{
  // set defaults;

  load();
}

void Preferences::load()
{
  QSettings settings;

  library_include_subfolders =
      settings.value(LIBRARY_INCLUDE_SUBFOLDERS).toBool();
}

void Preferences::save()
{
  QSettings settings;

  settings.setValue(LIBRARY_INCLUDE_SUBFOLDERS, library_include_subfolders);
}
} // namespace PhotoStage
