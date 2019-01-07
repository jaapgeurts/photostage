#include <QVBoxLayout>

#include "develophistogrammodule.h"

namespace PhotoStage
{
DevelopHistogramModule::DevelopHistogramModule(QWidget* parent) :
    DevelopModule(parent),
    mHistogram(new Widgets::Histogram(this))
{
    setLayout(new QVBoxLayout(this));
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->addWidget(mHistogram);
}

void DevelopHistogramModule::setPhoto(Photo photo)
{
    DevelopModule::setPhoto(photo);

    //mHistogram->setImageData(photo->libraryPreview());
}

void DevelopHistogramModule::recalculate()
{
    mHistogram->recalculate();
}
}
