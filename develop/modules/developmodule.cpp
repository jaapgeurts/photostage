#include "developmodule.h"

namespace PhotoStage
{
DevelopModule::DevelopModule(QWidget* parent) : QWidget(parent)
{
}

void DevelopModule::setPhoto(Photo photo)
{
    mPhoto = photo;
}

const Photo DevelopModule::photo() const
{
    return mPhoto;
}
}
