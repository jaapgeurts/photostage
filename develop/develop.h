#ifndef DEVELOP_H
#define DEVELOP_H

#include "module.h"
#include "modules/develophistogrammodule.h"
#include "modules/rawmodule.h"
#include "modules/basicmodule.h"


namespace Ui {
class Develop;
}

class Develop : public Module
{
    Q_OBJECT

public:
    explicit Develop(QWidget *parent = 0);
    ~Develop();

    QRect lightGap();

private:
    Ui::Develop *ui;
    DevelopHistogramModule *mHistogramModule;
    RawModule *mRawModule;
    BasicModule* mBasicModule;

};

#endif // DEVELOP_H
