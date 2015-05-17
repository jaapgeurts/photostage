#include <QMenu>
#include <QDebug>
#include <QStyle>
#include <QStyleOptionButton>

#include "imagedbtile.h"
#include "widgets/tileview.h"
#include "sqlphotoinfo.h"

ImageDbTile::ImageDbTile(TileView *parent) : AbstractTile(parent)
{

    mFontAwesome = QFont("FontAwesome",15);
}

void ImageDbTile::render(QPainter &painter, TileInfo& tileInfo, const QVariant &data)
{
    int w = painter.window().width();
    int h = painter.window().height();
    //   painter.drawLine(0,0,w-1,h-1);
    //   painter.drawLine(0,h,w-1,0);

    float ratio = 0.70f;
    int wf = (int)(w * ratio); // width frame
    int hf = (int)(h * ratio); // height frame

    // TODO: Load fonts in advance??
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
        painter.setPen(QColor(Qt::darkGray).lighter(110));
        painter.setFont(QFont(QString("Verdana"),24,QFont::Bold));
        int fontHeight = painter.fontMetrics().height();
        painter.drawText(5,fontHeight-5,QString::number(info.id));

        painter.restore();

        // draw the image.
        QImage image = info.image;

        int wi = image.width(); // width of image
        int hi = image.height();

        QRect newSize = resizeToFrameKeepAspectRatio(QSize(wi,hi),QSize(wf,hf));

        newSize.translate((w-wf)/2,(h-hf)/2);

        painter.drawImage(newSize,image);

        painter.save();


        // Draw the rotate handles
        if (mLeftHover.contains(tileInfo.index))
            painter.setPen(QColor(Qt::darkGray).lighter(180));
        else
            painter.setPen(QColor(Qt::darkGray).lighter(110));
        painter.setFont(mFontAwesome);
        painter.drawText(5,h-1-10,QString("%1").arg(QChar(0xf01e))); // rotate right
        painter.drawText(w-1-20,h-1-10,QString("%1").arg(QChar(0xf0e2))); // rotate left


        painter.restore();

        // draw checkbox
        if (tileInfo.tileState & (TileInfo::TileStateChecked | TileInfo::TileStateUnchecked))
        {
            QStyleOptionButton option;

            option.initFrom(parent());
            option.state = QStyle::State_Enabled;
            option.state |= tileInfo.tileState & TileInfo::TileStateChecked ? QStyle::State_On : QStyle::State_Off;
            option.rect.setRect(5,5,25,25);

            parent()->style()->drawPrimitive(QStyle::PE_IndicatorCheckBox,&option,&painter,parent());
        }

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

bool ImageDbTile::mouseMoveEvent(QMouseEvent*event,TileInfo& info)
{
    int x = event->pos().x();
    int y = event->pos().y();

    qDebug () << "Cell mouse ("<<x<<","<<y<<")";

    if (x < 30 && y > info.height - 30)
    {
        if (!mLeftHover.contains(info.index))
        {
            mLeftHover.insert(info.index,true);
            update();
        }
    }
    else
    {
        if (mLeftHover.contains(info.index))
        {
            mLeftHover.remove(info.index);
            update();
        }
    }
}

QRect ImageDbTile::resizeToFrameKeepAspectRatio(const QSize& src, const QSize& destFrame)
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

