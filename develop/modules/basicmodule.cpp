#include "basicmodule.h"
#include "ui_BasicModule.h"

BasicModule::BasicModule(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BasicModule)
{
    ui->setupUi(this);
}

BasicModule::~BasicModule()
{
    delete ui;
}
