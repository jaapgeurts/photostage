#include <QPainter>

#include "loupeview.h"

LoupeView::LoupeView(QWidget *parent) : QWidget(parent)
{
    mPhoto = NULL;
}

void LoupeView::setPhoto(Photo *photo)
{
    mPhoto = photo;
    setMinimumSize(mPhoto->rawImage().size());
}

void LoupeView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if (mPhoto != NULL && !mPhoto->rawImage().isNull())
    {

        painter.drawImage(0,0,mPhoto->rawImage());
    }
}
