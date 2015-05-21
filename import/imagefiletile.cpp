
#include "imagefiletile.h"

ImageFileTile::ImageFileTile(TileView *parent) : AbstractTile(parent)
{

}


void ImageFileTile::render(QPainter& painter,const TileInfo& tileInfo, const QVariant& data)
{
    int w = painter.window().width();
    int h = painter.window().height();
    //   painter.drawLine(0,0,w-1,h-1);
    //   painter.drawLine(0,h,w-1,0);

//    painter.setFont(QFont("helvetica",18));
    // painter.drawText(20,30,data.toString());

    // draw the selection background
    if ((tileInfo.tileState & TileInfo::TileStateSelected) == TileInfo::TileStateSelected)
        painter.setBrush(QBrush(QColor(Qt::darkGray).lighter(180),Qt::SolidPattern));
    else
        painter.setBrush(QBrush(QColor(Qt::darkGray),Qt::SolidPattern));
    // draw the tile
    painter.setPen(QColor(Qt::darkGray));
    painter.drawRect(1,1,w-2,h-2);
    painter.setBrush(Qt::NoBrush);

    // draw the bevel
    painter.setPen(QColor(Qt::gray));
    // draw clock wise
    painter.drawLine(0,h-1,0,0); //left side
    painter.drawLine(0,0,w-1,0); // top side
    painter.setPen(QColor(Qt::black));
    painter.drawLine(w-1,0,w-1,h-1);// right side
    painter.drawLine(w-1,h-1,0,h-1); // bottom side

    QImage image;

    if (!data.isNull())
    {
        image = data.value<QImage>();

        int iw = image.width();
        int ih = image.height();

        float ratio = (float)iw/(float)ih;

        int x=0;
        int y=0;
        int nh = h;
        int nw = w;
        if (ratio > 1.0f) {
            nh = h / ratio;
            y = (h-nh) / 2;
        }
        else {
            nw = w * ratio;
            x = (w-nw) / 2;
        }
        painter.drawImage(QRect(x,y,nw,nh),image);
    }
    else
    {
        painter.setPen(QColor(Qt::blue));
        painter.drawLine(0,0,w-1,h-1);
        painter.drawLine(0,h-1,w-1,0);
    }
}

