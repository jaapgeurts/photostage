#include "developmodule.h"

DevelopModule::DevelopModule(QWidget* parent) : QWidget(parent)
{
}

void DevelopModule::setPhoto(Photo* const photo)
{
    mPhoto = photo;
}

const Photo* DevelopModule::photo() const
{
    return mPhoto;
}
