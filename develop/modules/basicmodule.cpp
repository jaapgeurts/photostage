#include <QDebug>

#include <math.h>

#include "constants.h"
#include "engine/colortransform.h"
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
    //   qDebug() << "exposure value" << value;

    Image          img = mBasicOperation.execute(mPhoto.originalImage(), value / 9.0);

    ColorTransform tr1 = ColorTransform::getTransform("DevToQ", WORKING_COLOR_SPACE, "sRGB",
            ColorTransform::FORMAT_BGR48_PLANAR, ColorTransform::FORMAT_RGB32);
    QImage         qimg = tr1.transformToQImage(img);

    //    QImage qimg = img.toQImage();

    //    ColorTransform tr1 = ColorTransform::getTransform("DevToQ", WORKING_COLOR_SPACE, "sRGB",
    //            ColorTransform::FORMAT_RGB32, ColorTransform::FORMAT_RGB32);
    //    QImage         qimg = tr1.transformQImage(qimg2);

    mPhoto.setDevelopPreviewsRGB(qimg);

    // scale image before settings as preview.
    mPhoto.setLibraryPreview(img.toPreviewImage());

    emit parametersAdjusted();
}
}
