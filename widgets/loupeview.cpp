#include <QPainter>

#include "loupeview.h"

LoupeView::LoupeView(QWidget *parent) : QWidget(parent)
{
    mPhoto = NULL;
}

void LoupeView::setPhoto(Photo *photo)
{
    mPhoto = photo;
    setMinimumSize(mPhoto->libraryPreview().size());
}

void LoupeView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // TODO: should convert to the monitor profile here.
    if (mPhoto != NULL && !mPhoto->libraryPreviewsRGB().isNull())
    {

        painter.drawImage(0,0,mPhoto->libraryPreviewsRGB());
    }
}
