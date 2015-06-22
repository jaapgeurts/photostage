#include <QPainter>

#include "loupeview.h"
#include "utils.h"

namespace PhotoStage
{
    LoupeView::LoupeView(QWidget* parent)
        : QWidget(parent),
        mPhoto(NULL),
        mPanning(false),
        mPhotoTopLeft(0, 0),
        mZoomMode(ZoomFit)
    {
        setMinimumSize(200, 200);
        setSizePolicy(QSizePolicy::MinimumExpanding,
            QSizePolicy::MinimumExpanding);
        setCursor(QCursor(Qt::OpenHandCursor));
    }

    void LoupeView::setZoomMode(LoupeView::ZoomMode zoomMode)
    {
        mZoomMode = zoomMode;
        update();
    }

    void LoupeView::setPhoto(Photo* photo)
    {
        mPhoto = photo;
        ensureBestPosition();
        update();
    }

    void LoupeView::paintEvent(QPaintEvent* event)
    {
        QPainter painter(this);


        if (mPhoto == NULL)
            return;

        // TODO: should convert to the monitor profile here.
        QImage img = mPhoto->libraryPreviewsRGB();
        QRect  rect;
        rect.setTopLeft(mPhotoTopLeft);

        if (mPhoto != NULL && !img.isNull())
        {
            switch (mZoomMode)
            {
                case ZoomFit:
                    rect.setWidth(width());
                    rect.setHeight(height());
                    break;

                case Zoom25:
                    rect.setWidth(img.width() / 4);
                    rect.setHeight(img.height() / 4);
                    break;

                case Zoom50:
                    rect.setWidth(img.width() / 2);
                    rect.setHeight(img.height() / 2);
                    break;

                case Zoom100:
                    rect.setWidth(img.width());
                    rect.setHeight(img.height());
                    break;

                case Zoom150:
                    rect.setWidth((int)(img.width() * 1.5f));
                    rect.setHeight((int)(img.height() * 1.5f));
                    break;

                case Zoom200:
                    rect.setWidth(img.width() * 2);
                    rect.setHeight(img.height() * 2);
                    break;

                case Zoom300:
                    rect.setWidth(img.width() * 3);
                    rect.setHeight(img.height() * 3);
                    break;
            }

            QRect photoFinalDimension = fitFrame(img.size(), rect.size());

            painter.drawImage(photoFinalDimension, img);
        }
    }

    void LoupeView::mousePressEvent(QMouseEvent* event)
    {
        setCursor(QCursor(Qt::ClosedHandCursor));
        mPanning            = true;
        mMousePressLocation = event->pos();
    }

    void LoupeView::mouseReleaseEvent(QMouseEvent* event)
    {
        setCursor(QCursor(Qt::OpenHandCursor));
        mPanning       = false;
        mPhotoTopLeft -=  mMousePressLocation - event->pos();
        update();
    }

    void LoupeView::mouseMoveEvent(QMouseEvent* event)
    {
        if (mPanning)
        {
            mPhotoTopLeft -=  mMousePressLocation - event->pos();

            ensureBestPosition();

            mMousePressLocation = event->pos();
            update();
        }
    }

    void LoupeView::ensureBestPosition()
    {
        if (mPhotoTopLeft.x() > 0)
            mPhotoTopLeft.setX(0);

        if (mPhotoTopLeft.y() > 0)
            mPhotoTopLeft.setY(0);

        int hiddenx = width() - mPhoto->libraryPreviewsRGB().width();
        int hiddeny = height() - mPhoto->libraryPreviewsRGB().height();

        if (mPhotoTopLeft.x() < hiddenx)
            mPhotoTopLeft.setX(hiddenx);

        if (mPhotoTopLeft.y() < hiddeny)
            mPhotoTopLeft.setY(hiddeny);
    }
}