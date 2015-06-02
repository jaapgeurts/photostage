#ifndef HISTOGRAMMODULE_H
#define HISTOGRAMMODULE_H

#include <QObject>

#include "librarymodule.h"
#include "widgets/histogram.h"


class HistogramModule : public LibraryModule
{
public:
    HistogramModule(QWidget * parent);

    void setPhotos(const QList<Photo *> &list);

private:
    Histogram *mHistogram;

    PhotoData loadImage(const QImage &inImage);
};

#endif // HISTOGRAMMODULE_H
