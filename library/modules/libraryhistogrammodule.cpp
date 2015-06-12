#include <QVBoxLayout>
#include <QDebug>

#include "libraryhistogrammodule.h"

LibraryHistogramModule::LibraryHistogramModule(QWidget* parent) : LibraryModule(parent)
{
    mHistogram = new Histogram(this);
    setLayout(new QVBoxLayout(this));
    layout()->setContentsMargins(0,0,0,0);
    layout()->addWidget(mHistogram);
}

void LibraryHistogramModule::setPhotos(const QList<Photo*> &list)
{
    LibraryModule::setPhotos(list);

    if (list.size() == 1)
    {
        Photo* photo = list.at(0);

        if (!photo->libraryPreview().isNull())
        {
            //PhotoData image = loadImage(photo->libraryPreview());
            //mHistogram->setImageData(photo->libraryPreview());
        }
    }
}
