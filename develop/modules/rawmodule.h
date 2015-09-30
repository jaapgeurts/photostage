#ifndef PHOTOSTAGE_RAWMODULE_H
#define PHOTOSTAGE_RAWMODULE_H

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

        void setPhoto(Photo& photo);

    private:

        Ui::RawModule* ui;
        void setValues();
};
}

#endif // PHOTOSTAGE_RAWMODULE_H
