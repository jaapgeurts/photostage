
#include "imagefilecellrenderer.h"

ImageFileCellRenderer::ImageFileCellRenderer()
{

}


void ImageFileCellRenderer::render(QPainter& painter, const QVariant& data)
{
    int w = painter.window().width();
    int h = painter.window().height();
    //   painter.drawLine(0,0,w-1,h-1);
    //   painter.drawLine(0,h,w-1,0);

    painter.setFont(QFont("helvetica",18));
    // painter.drawText(20,30,data.toString());

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
