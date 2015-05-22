#include <QDir>

#include "imagefiletile.h"
#include "previewinfo.h"

ImageFileTile::ImageFileTile(TileView *parent) : AbstractTile(parent)
{

}


void ImageFileTile::render(QPainter& painter,const TileInfo& tileInfo, const QVariant& data)
{
    int w = painter.window().width();
    int h = painter.window().height();
    //   painter.drawLine(0,0,w-1,h-1);
    //   painter.drawLine(0,h,w-1,0);

    // draw the selection background
    if ((tileInfo.tileState & TileInfo::TileStateSelected) == TileInfo::TileStateSelected)
        painter.setBrush(QBrush(QColor(Qt::darkGray).lighter(180),Qt::SolidPattern));
    else
        painter.setBrush(QBrush(QColor(Qt::darkGray),Qt::SolidPattern));
    // draw the tile
    painter.setPen(QColor(Qt::darkGray));
    painter.drawRect(1,1,w-2,h-2);
    painter.setBrush(Qt::NoBrush);

    // draw the bevel of the tile
    painter.setPen(QColor(Qt::gray));
    // draw clock wise
    painter.drawLine(0,h-1,0,0); //left side
    painter.drawLine(0,0,w-1,0); // top side
    painter.setPen(QColor(Qt::black));
    painter.drawLine(w-1,0,w-1,h-1);// right side
    painter.drawLine(w-1,h-1,0,h-1); // bottom side

    PreviewInfo info;

    if (!data.isNull())
    {
        info = data.value<PreviewInfo>();
        QImage image = info.image;
        if (!image.isNull())
        {

            int wi = image.width();
            int hi = image.height();

            float ratio = 0.70f;
            int wf = (int)(w * ratio); // width frame
            int hf = (int)(h * ratio); // height frame

            QRect newSize = resizeToFrameKeepAspectRatio(QSize(wi,hi),QSize(wf,hf));

            // move the frame to the center
            newSize.translate((w-wf)/2,(h-hf)/2);
            // and draw the image
            painter.drawImage(newSize,image);

            // draw border around image
            painter.setPen(QColor(Qt::black));
            painter.drawRect(newSize);


            // draw the file name
            QFont fileNameFont = QFont("helvetica",10);
            painter.setFont(fileNameFont);
            painter.setPen(QColor(Qt::lightGray));
            QString name = info.filePath.mid(info.filePath.lastIndexOf(QDir::separator())+1);
            QFontMetrics m(fileNameFont);
            int textWidth = m.width(name);
            painter.drawText((w-textWidth)/2,h-5,name);
        }
        else
        {
            // invalid image
            painter.setPen(QColor(Qt::red));
            painter.drawLine(0,0,w-1,h-1);
            painter.drawLine(0,h-1,w-1,0);
        }

    }
    else
    {
        // TODO: draw missing picture
        painter.setPen(QColor(Qt::blue));
        painter.drawLine(0,0,w-1,h-1);
        painter.drawLine(0,h-1,w-1,0);
    }


}

/* fits the source frame into the destination frame and
 * centers the image */
QRect ImageFileTile::resizeToFrameKeepAspectRatio(const QSize& src, const QSize& destFrame)

{

    int ws = src.width(); // width source;
    int hs = src.height();

    int wd = destFrame.width(); // width destination
    int hd = destFrame.height();

    float ratio = (float)ws/(float)hs;

    int x=0;
    int y=0;
    int hn = hd; // new height
    int wn = wd; // new width
    if (ratio > 1.0f) {
        hn = hd / ratio;
        y = (hd-hn) / 2;
    }
    else {
        wn = wd * ratio;
        x = (wd-wn) / 2;
    }
    return QRect(x,y,wn,hn);
}

