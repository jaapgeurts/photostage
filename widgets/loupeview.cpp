#include <QPainter>
#include <QPoint>
#include <QRect>

#include "loupeview.h"
#include "utils.h"

namespace PhotoStage
{
LoupeView::LoupeView(QWidget* parent)
    : QWidget(parent),
    mPanning(false),
    mPhotoTopLeft(0, 0),
    mZoomMode(ZoomFit)
{
    setMinimumSize(200, 200);
    setSizePolicy(QSizePolicy::MinimumExpanding,
        QSizePolicy::MinimumExpanding);
    setCursor(QCursor(Qt::OpenHandCursor));

    //    QImage image(40,40,QImage::Format_RGB32);
    //    QPainter p1(&image);
    //    p1.setCompositionMode(QPainter::CompositionMode_Source);
    //    p1.fillRect(0,0,20,20,gray1);
    //    p1.fillRect(20,0,20,20,gray2);
    //    p1.fillRect(0,20,20,20,gray2);
    //    p1.fillRect(20,20,20,20,gray1);
    //    mCheckeredBrush = QBrush(image);
}

void LoupeView::setZoomMode(LoupeView::ZoomMode zoomMode)
{
    mZoomMode = zoomMode;
    update();
}

void LoupeView::setPhoto(Photo photo)
{
    mPhoto = photo;
    ensureBestPosition();
    update();
}

void LoupeView::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    if (mPhoto.isNull())
        return;

    //    painter.fillRect(QRect(0,0,width(),height()),mCheckeredBrush);
    painter.fillRect(QRect(0, 0, width(), height()), Qt::darkGray);

    // TODO: should convert to the monitor profile here.
    QImage img = mPhoto.libraryPreviewsRGB();
    QRect  rect;
    float  zoomFitScale = 0.9;

    if (!img.isNull())
    {
        switch (mZoomMode)
        {
            case ZoomFit:
                rect.setWidth((int)(width() * zoomFitScale));
                rect.setHeight((int)(height() * zoomFitScale));
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

        QRect  photoFinalDimension = fitFrame(img.size(), rect.size());

        if (photoFinalDimension.width() < width())
            photoFinalDimension.moveLeft((width() - photoFinalDimension.width()) / 2);

        if (photoFinalDimension.height() < height())
            photoFinalDimension.moveTop((height() - photoFinalDimension.height()) / 2);

        QPoint topleft = photoFinalDimension.topLeft();
        topleft.setX(topleft.x() - 5);
        topleft.setY(topleft.y() - 5);
        QPoint bottomright = photoFinalDimension.bottomRight();
        bottomright.setX(bottomright.x() + 10);
        bottomright.setY(bottomright.y() + 15);

        QLinearGradient gradient(topleft, bottomright);
        //        gradient.setStart(photoFinalDimension.topLeft());
        //        gradient.setFinalStop(0,photoFinalDimension.bottom()+15);
        QColor gray1 = QColor(Qt::darkGray).darker(150);
        QColor gray2 = QColor(Qt::darkGray).darker(200);
        gray1.setAlpha(30);
        gradient.setColorAt((qreal)0, gray2);
        gradient.setColorAt((qreal)0.1, gray1);
        gradient.setColorAt((qreal)0.9, gray1);
        gradient.setColorAt((qreal)1, gray2);

        QBrush brush(gradient);
        painter.setBrush(brush);
        QPen   pen;
        pen.setStyle(Qt::NoPen);
        painter.setPen(pen);

        painter.drawRoundedRect(QRect(topleft, bottomright), 5, 12);

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

    int hiddenx = width() - mPhoto.libraryPreviewsRGB().width();
    int hiddeny = height() - mPhoto.libraryPreviewsRGB().height();

    if (mPhotoTopLeft.x() < hiddenx)
        mPhotoTopLeft.setX(hiddenx);

    if (mPhotoTopLeft.y() < hiddeny)
        mPhotoTopLeft.setY(hiddeny);
}
}
