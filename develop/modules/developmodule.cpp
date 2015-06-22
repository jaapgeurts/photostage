#include "developmodule.h"

namespace PhotoStage
{
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
}