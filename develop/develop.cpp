#include "develop.h"
#include "ui_develop.h"

Develop::Develop(QWidget *parent) :
    Module(parent),
    ui(new Ui::Develop)
{
    ui->setupUi(this);

    // add panels for development.

    // Histogram
    mHistogramModule = new DevelopHistogramModule(ui->DevelopPanel);
    ui->DevelopPanel->addPanel("Histogram",mHistogramModule);

    // Raw
    mRawModule = new RawModule(ui->DevelopPanel);
    ui->DevelopPanel->addPanel("RAW",mRawModule);

    // Normal
    mBasicModule = new BasicModule(ui->DevelopPanel);
    ui->DevelopPanel->addPanel("Basic",mBasicModule);


}

Develop::~Develop()
{
    delete ui;
}

QRect Develop::lightGap()
{
// TODO: implement this
}
