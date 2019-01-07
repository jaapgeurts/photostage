#include <QVBoxLayout>
#include <QDebug>

#include "libraryhistogrammodule.h"

namespace PhotoStage
{
LibraryHistogramModule::LibraryHistogramModule(QWidget* parent) :
    LibraryModule(parent)
{
    mHistogram = new Widgets::Histogram(this);
    setLayout(new QVBoxLayout(this));
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->addWidget(mHistogram);
}

void LibraryHistogramModule::setPhoto(Photo& photo)
{
    LibraryModule::setPhoto(photo);

    if (photo.isNull())
    {
        //        qDebug() << "HISTOGRAM: Library is null";

        return;
    }
    else if (photo.originalImage().isNull())
    {
        //        qDebug() << "HISTOGRAM: Original image is null";
        mHistogram->clear();
        return;
    }

    const Image image = photo.originalImage();

    if (!image.isNull())
    {
        //PhotoData image = loadImage(photo->libraryPreview());
        mHistogram->setImageData(image.data(), image.width(), image.height());
    }
}
}
