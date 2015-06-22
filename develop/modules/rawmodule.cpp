#include "rawmodule.h"
#include "ui_rawmodule.h"

namespace PhotoStage
{
RawModule::RawModule(QWidget* parent) :
    DevelopModule(parent),
    ui(new Ui::RawModule)
{
    ui->setupUi(this);
}
}