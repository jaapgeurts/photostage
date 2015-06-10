#ifndef DEVELOPHISTOGRAMMODULE_H
#define DEVELOPHISTOGRAMMODULE_H

#include "developmodule.h"
#include "widgets/histogram.h"

class DevelopHistogramModule : public DevelopModule
{
    Q_OBJECT

    public:

        DevelopHistogramModule(QWidget* parent = 0);

        void setPhoto(Photo* const photo);

    public slots:

        void recalculate();

    private:

        Histogram* mHistogram;
};

#endif // DEVELOPHISTOGRAMMODULE_H
