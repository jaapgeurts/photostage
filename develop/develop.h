#ifndef DEVELOP_H
#define DEVELOP_H

#include "module.h"

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
};

#endif // DEVELOP_H
