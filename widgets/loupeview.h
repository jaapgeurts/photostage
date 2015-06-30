#ifndef LOUPEVIEW_H
#define LOUPEVIEW_H

#include <QWidget>
#include <QPaintEvent>

#include "photo.h"

namespace PhotoStage
{
    class LoupeView : public QWidget
    {
        Q_OBJECT

        public:

            enum ZoomMode
            {
                ZoomFit = 1,
                Zoom25,
                Zoom50,
                Zoom100,
                Zoom150,
                Zoom200,
                Zoom300
            };

            explicit LoupeView(QWidget* parent = 0);

            void setZoomMode(ZoomMode zoomMode);

        signals:

        public slots:

            void setPhoto(Photo photo);

        protected:

            void paintEvent(QPaintEvent* event);

            //    void enterEvent(QEvent *event);
            //    void leaveEvent(QEvent* event);
            void mousePressEvent(QMouseEvent* event);
            void mouseReleaseEvent(QMouseEvent* event);
            void mouseMoveEvent(QMouseEvent* event);

        private:

            Photo   mPhoto;
            bool     mPanning;
            QPoint   mMousePressLocation;
            QPoint   mPhotoTopLeft;
            ZoomMode mZoomMode;
//            QBrush   mCheckeredBrush;

            void ensureBestPosition();
    };
}

#endif // LOUPEVIEW_H
