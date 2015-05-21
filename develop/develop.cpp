#include "develop.h"
#include "ui_develop.h"

Develop::Develop(QWidget *parent) :
    Module(parent),
    ui(new Ui::Develop)
{
    ui->setupUi(this);
}

Develop::~Develop()
{
    delete ui;
}

QRect Develop::lightGap()
{
// TODO: implement this
}
