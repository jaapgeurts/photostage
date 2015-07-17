#ifndef LIBRARYHISTOGRAMMODULE_H
#define LIBRARYHISTOGRAMMODULE_H

#include <QObject>

#include "librarymodule.h"
#include "widgets/histogram.h"

namespace PhotoStage
{
    class LibraryHistogramModule : public LibraryModule
    {
        public:

            LibraryHistogramModule(QWidget* parent);

            void setPhoto(Photo& photo);

        private:

            Histogram* mHistogram;

            //    PhotoData loadImage(const QImage &inImage);
    };
}
#endif // LIBRARYHISTOGRAMMODULE_H
