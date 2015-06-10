#include <QSettings>

#include "preferences.h"

#define LIBRARY_INCLUDE_SUBFOLDERS "library/include_subfolders"

Preferences* Preferences::mPreferences = NULL;

Preferences* Preferences::instance()
{
    if (mPreferences == NULL)
        mPreferences = new Preferences(NULL);
    return mPreferences;
}

void Preferences::load()
{
    QSettings settings;

    library_include_subfolders = settings.value(LIBRARY_INCLUDE_SUBFOLDERS).toBool();
}

void Preferences::save()
{
    QSettings settings;

    settings.setValue(LIBRARY_INCLUDE_SUBFOLDERS,library_include_subfolders);
}

Preferences::Preferences(QObject* parent) : QObject(parent)
{
    // set defaults;
    library_include_subfolders = true;

    load();
}