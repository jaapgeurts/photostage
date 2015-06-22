#ifndef RAWMODULE_H
#define RAWMODULE_H

#include <QWidget>
#include "developmodule.h"

namespace Ui
{
    class RawModule;
}

namespace PhotoStage
{
    class RawModule : public DevelopModule
    {
        Q_OBJECT

        public:

            RawModule(QWidget* parent = 0);

        private:

            Ui::RawModule* ui;
    };
}

#endif // RAWMODULE_H