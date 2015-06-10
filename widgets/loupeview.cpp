#include <QPainter>

#include "loupeview.h"

LoupeView::LoupeView(QWidget* parent)
    : QWidget(parent),
    mPhoto(NULL),
    mPanning(false),
    mColorTransform(ColorTransform(WORKING_COLOR_SPACE,"sRGB")),
    mPhotoTopLeft(0,0)
{
    setMinimumSize(200,200);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setCursor(QCursor(Qt::OpenHandCursor));
}

void LoupeView::setPhoto(Photo* photo)
{
    mPhoto = photo;
}

void LoupeView::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    if (mPhoto == NULL)
        return;
    // TODO: should convert to the monitor profile here.
    QImage img = mPhoto->libraryPreviewsRGB(mColorTransform);

    if (mPhoto != NULL && !img.isNull())
    {
        painter.drawImage(mPhotoTopLeft,img);
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

        if (mPhotoTopLeft.x() > 0)
            mPhotoTopLeft.setX(0);

        if (mPhotoTopLeft.y() > 0)
            mPhotoTopLeft.setY(0);

        int hiddenx = width() - mPhoto->libraryPreviewsRGB(mColorTransform).width();
        int hiddeny = height() - mPhoto->libraryPreviewsRGB(mColorTransform).height();

        if (mPhotoTopLeft.x() < hiddenx)
            mPhotoTopLeft.setX(hiddenx);

        if (mPhotoTopLeft.y() < hiddeny)
            mPhotoTopLeft.setY(hiddeny);

        mMousePressLocation = event->pos();
        update();
    }
}
