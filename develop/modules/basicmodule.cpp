#include <QDebug>

#include <math.h>

#include "basicmodule.h"
#include "ui_BasicModule.h"

namespace PhotoStage
{
BasicModule::BasicModule(QWidget* parent) :
    DevelopModule(parent),
    ui(new Ui::BasicModule)
{
    ui->setupUi(this);
}

BasicModule::~BasicModule()
{
    delete ui;
}

void BasicModule::onExposureChanged(int value)
{
    qDebug() << "exposure value" << value;

    Image img = mBasicOperation.execute(mPhoto.originalImage(), value / 9.0);
    mPhoto.setLibraryPreview(img.toQImage());

    //    Image img = mPhoto->original().clone();

    //    mPhoto->setLibraryPreview(img);
    emit parametersAdjusted();
}
}
