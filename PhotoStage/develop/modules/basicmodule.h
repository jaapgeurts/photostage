#ifndef PHOTOSTAGE_BASICMODULE_H
#define PHOTOSTAGE_BASICMODULE_H

#include "developmodule.h"
#include "engine/basicoperation.h"

namespace Ui
{
class BasicModule;
}

namespace PhotoStage
{
class BasicModule : public DevelopModule
{
    Q_OBJECT

    public:

        explicit BasicModule(QWidget* parent = 0);
        ~BasicModule();

    public slots:

        void onExposureChanged(int value);

    private:

        Ui::BasicModule* ui;
        BasicOperation   mBasicOperation;
};
}
#endif // PHOTOSTAGE_BASICMODULE_H
