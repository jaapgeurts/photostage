#include "metadatamodule.h"
#include "ui_metadatamodule.h"

namespace PhotoStage
{
MetaDataModule::MetaDataModule(QWidget* parent) :
    LibraryModule(parent),
    ui(new Ui::MetaDataModule)
{
    ui->setupUi(this);
}
}
