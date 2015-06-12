#include <QVBoxLayout>

#include "develophistogrammodule.h"

DevelopHistogramModule::DevelopHistogramModule(QWidget* parent) :
    DevelopModule(parent),
    mHistogram(new Histogram(this))
{
    setLayout(new QVBoxLayout(this));
    layout()->setContentsMargins(0,0,0,0);
    layout()->addWidget(mHistogram);
}

void DevelopHistogramModule::setPhoto(Photo * const photo)
{
    DevelopModule::setPhoto(photo);

    //mHistogram->setImageData(photo->libraryPreview());
}

void DevelopHistogramModule::recalculate()
{
    mHistogram->recalculate();
}
