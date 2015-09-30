#include <QPainter>
#include <QPoint>
#include <QRect>
#include <QDebug>
#include <QGraphicsDropShadowEffect>

#include "developview.h"
#include "utils.h"

namespace Widgets
{
DevelopView::DevelopView(QWidget* parent)
    : QWidget(parent),
    mPanning(false),
    mPhotoTopLeft(0, 0),
    mZoomMode(ZoomFit),
    mInfoMode(InfoOff)
{
    setMinimumSize(200, 200);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
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

void DevelopView::setZoomMode(DevelopView::ZoomMode zoomMode)
{
    mZoomMode = zoomMode;

    ensureCorrectPosition();

    update();
}

void DevelopView::setPhoto(QImage image)
{
    mPhoto = image;
    computeZoomToFitScaleFactor();
    ensureCorrectPosition();
    update();
}

void DevelopView::setInfoMode(DevelopView::InfoMode mode)
{
    mInfoMode = mode;
    update();
}

void DevelopView::cycleInfoMode()
{
    int i = mInfoMode;

    i++;

    if (i >= InfoLast)
        i = InfoOff;

    mInfoMode = static_cast<InfoMode>(i);
    update();
}

void DevelopView::paintEvent(QPaintEvent* event)
{
    if (mPhoto.isNull())
        return;

    QPainter painter(this);

    //    painter.fillRect(QRect(0,0,width(),height()),mCheckeredBrush);
    // painter.fillRect(QRect(0, 0, width(), height()), Qt::darkGray);

    // TODO: should convert to the monitor profile here.

    QRect rect;

    rect.setTopLeft(mPhotoTopLeft);
    rect.setWidth((int)(mPhoto.width() * mZoomFactors[mZoomMode]));
    rect.setHeight((int)(mPhoto.height() * mZoomFactors[mZoomMode]));

    //  drawDropShadow();

    painter.drawImage(rect, mPhoto);
}

void DevelopView::drawDropShadow()
{
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
}

void DevelopView::mousePressEvent(QMouseEvent* event)
{
    setCursor(QCursor(Qt::ClosedHandCursor));
    mPanning            = true;
    mMousePressLocation = event->pos();
}

void DevelopView::mouseReleaseEvent(QMouseEvent* event)
{
    setCursor(QCursor(Qt::OpenHandCursor));
    mPanning       = false;
    mPhotoTopLeft -=  mMousePressLocation - event->pos();
    update();
}

void DevelopView::mouseMoveEvent(QMouseEvent* event)
{
    if (mPanning)
    {
        mPhotoTopLeft -=  mMousePressLocation - event->pos();

        ensureCorrectPosition();

        mMousePressLocation = event->pos();
        update();
    }
}

void DevelopView::resizeEvent(QResizeEvent*)
{
    computeZoomToFitScaleFactor();

    ensureCorrectPosition();

    update();
}

void DevelopView::computeZoomToFitScaleFactor()
{
    if (mPhoto.isNull())
        return;

    int ww = width();
    int wh = height();

    int iw = mPhoto.width();
    int ih = mPhoto.height();

    if (iw > ih)
        mZoomFactors[ZoomFit] = (float)ww / (float)iw * 0.9f;
    else
        mZoomFactors[ZoomFit] = (float)wh / (float)ih * 0.9f;
}

/*
   QImage applyEffectToImage(QImage src, QGraphicsEffect effect, int extent)
   {
   if(src.isNull()) return QImage(); //No need to do anything else!
   if(!effect) return src; //No need to do anything else!
   QGraphicsScene scene;
   QGraphicsPixmapItem item;
   item.setPixmap(QPixmap::fromImage(src));
   item.setGraphicsEffect(effect);
   scene.addItem(&item);
   QImage res(src.size()+QSize(extent2, extent2), QImage::Format_ARGB32);
   res.fill(Qt::transparent);
   QPainter ptr(&res);
   scene.render(&ptr, QRectF(), QRectF( -extent, -extent, src.width()+extent2, src.height()+extent*2 ) );
   return res;
   }
 */

void DevelopView::ensureCorrectPosition()
{
    if (mPhoto.isNull())
        return;

    int iw = mPhoto.width() * mZoomFactors[mZoomMode];
    int ih = mPhoto.height() * mZoomFactors[mZoomMode];
    int ww = width();
    int wh = height();

    if (iw < ww)
    {
        mPhotoTopLeft.setX((ww - iw) / 2);
    }
    else
    {
        int hiddenx = ww - iw;

        if (mPhotoTopLeft.x() > 0)
            mPhotoTopLeft.setX(0);

        if (mPhotoTopLeft.x() < hiddenx)
            mPhotoTopLeft.setX(hiddenx);
    }

    if (ih < wh)
    {
        mPhotoTopLeft.setY((wh - ih) / 2);
    }
    else
    {
        if (mPhotoTopLeft.y() > 0)
            mPhotoTopLeft.setY(0);

        int hiddeny = wh - ih;

        if (mPhotoTopLeft.y() < hiddeny)
            mPhotoTopLeft.setY(hiddeny);
    }
}
}
