#include <QPainter>
#include <QPoint>
#include <QRect>
#include <QDebug>

#include "loupeview.h"
#include "utils.h"

namespace PhotoStage
{
LoupeView::LoupeView(QWidget* parent)
    : QWidget(parent),
    mPanning(false),
    mPhotoTopLeft(0, 0),
    mZoomMode(ZoomFit),
    mInfoMode(InfoOff)
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

void LoupeView::setInfoMode(LoupeView::InfoMode mode)
{
    mInfoMode = mode;
    update();
}

void LoupeView::cycleInfoMode()
{
    int i = mInfoMode;

    i++;

    if (i >= InfoLast)
        i = InfoOff;

    mInfoMode = static_cast<InfoMode>(i);
    qDebug() << "cycling";
    update();
}

void LoupeView::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);

    if (mPhoto.isNull())
        return;

    //    painter.fillRect(QRect(0,0,width(),height()),mCheckeredBrush);
    painter.fillRect(QRect(0, 0, width(), height()), Qt::darkGray);

    // TODO: should convert to the monitor profile here.
    QImage img = mPhoto.libraryPreviewsRGB();
    QRect  rect;

    if (!img.isNull())
    {
        rect.setTopLeft(mPhotoTopLeft);
        rect.setWidth((int)(img.width() * mZoomFactors[mZoomMode]));
        rect.setHeight((int)(img.height() * mZoomFactors[mZoomMode]));

        /*
                // for border drop shadow
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

                painter.drawRoundedRect(QRect(topleft, bottomright), 5, 12); */

        painter.drawImage(rect, img);

        QFont font(painter.font());
        //font.setBold(true);
        font.setPixelSize(22);
        painter.setFont(font);

        QPen pen;
        pen.setStyle(Qt::SolidLine);
        pen.setColor(Qt::white);
        painter.setPen(pen);

        if (mInfoMode == InfoExposure)
        {
            const ExifInfo& ei = mPhoto.exifInfo();
            painter.drawText(40, 50, mPhoto.srcImagePath());
            painter.drawText(40, 80,
                ei.formatExposure() + ", ISO " + QString::number(ei.isoSpeed));
            painter.drawText(40, 110,
                QString::number(ei.focalLength) + "mm " + ei.lensName);
        }

        if (mInfoMode == InfoSpaceTime)
        {
            const ExifInfo& ei = mPhoto.exifInfo();
            painter.drawText(40, 50, mPhoto.srcImagePath());
            painter.drawText(40, 80, ei.dateTimeOriginal.toString());
            painter.drawText(40, 110, ei.formatDimension());
        }
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

void LoupeView::resizeEvent(QResizeEvent*)
{
    int ww = width();
    int wh = height();

    int iw = mPhoto.libraryPreviewsRGB().width();
    int ih = mPhoto.libraryPreviewsRGB().height();

    mZoomFactors[ZoomFit] = (float)ww / (float)iw * 0.9f;

    ensureBestPosition();

    update();
}

void LoupeView::ensureBestPosition()
{
    if (mPhotoTopLeft.x() > 0)
        mPhotoTopLeft.setX(0);

    if (mPhotoTopLeft.y() > 0)
        mPhotoTopLeft.setY(0);

    int hiddenx = width() - mPhoto.libraryPreviewsRGB().width()
        * mZoomFactors[mZoomMode];
    int hiddeny = height() - mPhoto.libraryPreviewsRGB().height()
        * mZoomFactors[mZoomMode];

    if (mPhotoTopLeft.x() < hiddenx)
        mPhotoTopLeft.setX(hiddenx);

    if (mPhotoTopLeft.y() < hiddeny)
        mPhotoTopLeft.setY(hiddeny);
}
}
