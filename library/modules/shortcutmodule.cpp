#include <QVBoxLayout>
#include "shortcutmodule.h"

namespace PhotoStage
{
ShortcutModule::ShortcutModule(QWidget* parent) : LibraryModule(parent)
{
    mWorkCollection = new QPushButton("Work Collection", this);
    mImportHistory  = new QComboBox(this);
    mImportHistory->insertItem(0, "No Previous Imports");
    mImportHistory->setEditable(false);

    QVBoxLayout* vboxLayout = new QVBoxLayout(this);
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    vboxLayout->addWidget(mWorkCollection);
    vboxLayout->addWidget(mImportHistory);
}
}