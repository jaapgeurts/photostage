#ifndef BASIC_H
#define BASIC_H

#include <QWidget>

#include "developmodule.h"

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
};
}
#endif // BASIC_H
