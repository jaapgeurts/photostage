#include "ui_geometrymodule.h"

#include "geometrymodule.h"

namespace PhotoStage
{
GeometryModule::GeometryModule(QWidget* parent) :
    DevelopModule(parent),
    ui(new Ui::GeometryModule)
{
    ui->setupUi(this);
}

void GeometryModule::onCropRotateClicked()
{
    emit cropRotateClicked();
}

void GeometryModule::onLockAspectClicked(bool enabled)
{
    emit lockAspectRatioClicked(enabled);
}

void GeometryModule::onAspectRatioChanged(int aspect)
{

    emit aspectRatioChanged();
}
}
