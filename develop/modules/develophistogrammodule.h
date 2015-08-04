#ifndef DEVELOPHISTOGRAMMODULE_H
#define DEVELOPHISTOGRAMMODULE_H

#include "developmodule.h"
#include "widgets/histogram.h"

namespace PhotoStage
{
    class DevelopHistogramModule : public DevelopModule
    {
        Q_OBJECT

        public:

            DevelopHistogramModule(QWidget* parent = 0);

            void setPhoto(Photo photo);

        public slots:

            void recalculate();

        private:

            Widgets::Histogram* mHistogram;
    };
}
#endif // DEVELOPHISTOGRAMMODULE_H
