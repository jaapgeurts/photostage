#ifndef DEVELOPHISTOGRAMMODULE_H
#define DEVELOPHISTOGRAMMODULE_H

#include "developmodule.h"
#include "widgets/histogram.h"

class DevelopHistogramModule : public DevelopModule
{
public:
    DevelopHistogramModule(QWidget *parent=0);

private:
    Histogram* mHistogram;
};

#endif // DEVELOPHISTOGRAMMODULE_H
