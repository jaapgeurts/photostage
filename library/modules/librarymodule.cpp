#include "librarymodule.h"

namespace PhotoStage
{
LibraryModule::LibraryModule(QWidget* parent) : QWidget(parent)
{
}

QMenu* LibraryModule::getMenu()
{
    return NULL;
}
}
