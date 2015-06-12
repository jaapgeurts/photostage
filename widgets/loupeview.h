#ifndef LOUPEVIEW_H
#define LOUPEVIEW_H

#include <QWidget>
#include <QPaintEvent>

#include "photo.h"

class LoupeView : public QWidget
{
    Q_OBJECT

    public:

        explicit LoupeView(QWidget* parent = 0);

    signals:

    public slots:

        void setPhoto(Photo* photo);

    protected:

        void paintEvent(QPaintEvent* event);

        //    void enterEvent(QEvent *event);
        //    void leaveEvent(QEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);

    private:

        Photo*         mPhoto;
        bool           mPanning;
        QPoint         mMousePressLocation;
        QPoint         mPhotoTopLeft;
};

#endif // LOUPEVIEW_H
