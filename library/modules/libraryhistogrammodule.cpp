#include <QVBoxLayout>
#include <QDebug>

#include "libraryhistogrammodule.h"

namespace PhotoStage
{
LibraryHistogramModule::LibraryHistogramModule(QWidget* parent) :
    LibraryModule(parent)
{
    mHistogram = new Histogram(this);
    setLayout(new QVBoxLayout(this));
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->addWidget(mHistogram);
}

void LibraryHistogramModule::setPhoto(Photo& photo)
{
    LibraryModule::setPhoto(photo);

    if (!photo.isNull() && !photo.libraryPreview().isNull())
    {
        //PhotoData image = loadImage(photo->libraryPreview());
        mHistogram->setImageData(photo.libraryPreview());
    }
    else
    {
        mHistogram->clear();
    }
}
}
