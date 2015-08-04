#ifndef LOUPEVIEW_H
#define LOUPEVIEW_H

#include <QWidget>
#include <QPaintEvent>

#include "photo.h"

namespace Widgets
{
class LoupeView : public QWidget
{
    Q_OBJECT

    public:

        enum ZoomMode
        {
            ZoomFit = 0,
            Zoom25,
            Zoom50,
            Zoom100,
            Zoom150,
            Zoom200,
            Zoom300,
            Zoom400,
            Zomm800,
            ZoomLast
        };

        enum InfoMode
        {
            InfoOff = 1,
            InfoSpaceTime,
            InfoExposure,
            InfoLast
        };

        explicit LoupeView(QWidget* parent = 0);

        void setZoomMode(ZoomMode zoomMode);

    signals:

    public slots:

        void setPhoto(PhotoStage::Photo photo);

        void setInfoMode(InfoMode mode);
        void cycleInfoMode();

    protected:

        void paintEvent(QPaintEvent*);

        //    void enterEvent(QEvent *event);
        //    void leaveEvent(QEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);

        void resizeEvent(QResizeEvent*);

    private:

        PhotoStage::Photo mPhoto;
        bool              mPanning;
        QPoint            mMousePressLocation;
        QPoint            mPhotoTopLeft;
        ZoomMode          mZoomMode;
        InfoMode          mInfoMode;
        //            QBrush   mCheckeredBrush;

        float mZoomFactors[ZoomLast] =
        { 0.9f, 0.25f, 0.5f, 1, 1.5f, 2.0f, 3.0f, 4.0f, 8.0f };

        void ensureCorrectPosition();
        void computeZoomToFitScaleFactor();
};
}

#endif // LOUPEVIEW_H
