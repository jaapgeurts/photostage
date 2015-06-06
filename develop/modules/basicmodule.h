#ifndef BASIC_H
#define BASIC_H

#include <QWidget>

namespace Ui {
class BasicModule;
}

class BasicModule : public QWidget
{
    Q_OBJECT

public:
    explicit BasicModule(QWidget *parent = 0);
    ~BasicModule();

private:
    Ui::BasicModule *ui;
};

#endif // BASIC_H
