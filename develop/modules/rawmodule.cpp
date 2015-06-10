#include "rawmodule.h"
#include "ui_rawmodule.h"

RawModule::RawModule(QWidget* parent) :
    DevelopModule(parent),
    ui(new Ui::RawModule)
{
    ui->setupUi(this);
}
