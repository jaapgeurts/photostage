#ifndef LOUPEVIEW_H
#define LOUPEVIEW_H

#include <QWidget>
#include <QPaintEvent>

#include "photo.h"

class LoupeView : public QWidget
{
    Q_OBJECT
public:
    explicit LoupeView(QWidget *parent = 0);

signals:

public slots:

    void setPhoto(Photo * photo);

protected:
    void paintEvent(QPaintEvent *event);

private:
    Photo * mPhoto;
};

#endif // LOUPEVIEW_H
