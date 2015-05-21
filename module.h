#ifndef MODULE_H
#define MODULE_H

#include <QWidget>
#include <QRect>

class Module : public QWidget
{
    Q_OBJECT

public:
    Module(QWidget* parent =0) : QWidget(parent) {}
    virtual ~Module() {}

    virtual QRect lightGap() = 0;
};

#endif // MODULE_H

