#include <QPainter>
#include <QPoint>
#include <QRect>
#include <QDebug>
#include <QGraphicsDropShadowEffect>

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

    ensureCorrectPosition();

    update();
}

void LoupeView::setPhoto(Photo photo)
{
    mPhoto = photo;
    computeZoomToFitScaleFactor();
    ensureCorrectPosition();
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
            QString         exp = ei.isoSpeed == nullptr ? "" : QString(", ISO %1").arg(ei.isoSpeed.value);
            painter.drawText(40, 80, ei.formatExposure() + exp);

            QString fl = ei.focalLength == nullptr ? "" : QString("%1mm ").arg(ei.focalLength.value);
            QString ln = ei.lensName == nullptr ? "" : ei.lensName.value;
            painter.drawText(40, 110, fl + ln);
        }

        if (mInfoMode == InfoSpaceTime)
        {
            const ExifInfo& ei = mPhoto.exifInfo();
            painter.drawText(40, 50, mPhoto.srcImagePath());

            if (ei.dateTimeOriginal != nullptr)
                painter.drawText(40, 80, ei.dateTimeOriginal.value.toString());
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

        ensureCorrectPosition();

        mMousePressLocation = event->pos();
        update();
    }
}

void LoupeView::resizeEvent(QResizeEvent*)
{
    computeZoomToFitScaleFactor();

    ensureCorrectPosition();

    update();
}

void LoupeView::computeZoomToFitScaleFactor()
{
    if (mPhoto.isNull())
        return;

    int ww = width();
    int wh = height();

    int iw = mPhoto.libraryPreviewsRGB().width();
    int ih = mPhoto.libraryPreviewsRGB().height();

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

void LoupeView::ensureCorrectPosition()
{
    if (mPhoto.isNull())
        return;

    int iw = mPhoto.libraryPreviewsRGB().width() * mZoomFactors[mZoomMode];
    int ih = mPhoto.libraryPreviewsRGB().height() * mZoomFactors[mZoomMode];
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
