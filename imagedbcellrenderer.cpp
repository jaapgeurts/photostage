#include <QMenu>

#include "imagedbcellrenderer.h"

#include "sqlphotoinfo.h"

ImageDbCellRenderer::ImageDbCellRenderer()
{

}

void ImageDbCellRenderer::render(QPainter &painter, const QVariant &data)
{
    int w = painter.window().width();
    int h = painter.window().height();
    //   painter.drawLine(0,0,w-1,h-1);
    //   painter.drawLine(0,h,w-1,0);

    float ratio = 0.80f;
    int wf = (int)(w * ratio); // width frame
    int hf = (int)(h * ratio); // height frame

    painter.setFont(QFont("helvetica",18));




    if (!data.isNull())
    {

        // draw the tile
        painter.setPen(QColor(Qt::darkGray));
        painter.setBrush(QBrush(QColor(Qt::darkGray),Qt::SolidPattern));
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

        SqlPhotoInfo info = data.value<SqlPhotoInfo>();

        // draw the id text
        painter.save();
        painter.setPen(QColor(Qt::darkGray).lighter());
        painter.setFont(QFont(QString("Verdana"),24,QFont::Bold));
        int fontHeight = painter.fontMetrics().height();
        painter.drawText(5,fontHeight,QString::number(info.id));

        painter.restore();

        // draw the image.
        QImage image = info.image;

        int wi = image.width(); // width of image
        int hi = image.height();

        QRect newSize = resizeToFrameKeepAspectRatio(QSize(wi,hi),QSize(wf,hf));

        newSize.translate((w-wf)/2,(h-hf)/2);

        painter.drawImage(newSize,image);
        // draw border around image
        // TODO: check if the rectange is on or outside the image
        painter.setPen(QColor(Qt::black));
        painter.drawRect(newSize);
    }
    else
    {
        painter.setPen(QColor(Qt::blue));
        painter.drawLine(0,0,w-1,h-1);
        painter.drawLine(0,h-1,w-1,0);
    }
}

void ImageDbCellRenderer::mouseReleaseEvent(QMouseEvent*event)
{
    QMenu m;;
    m.addAction("Awesome photo");
    m.exec();
}

QRect ImageDbCellRenderer::resizeToFrameKeepAspectRatio(const QSize& src, const QSize& destFrame)
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
