#include "filmstriptile.h"
#include "photo.h"
#include "utils.h"

namespace PhotoStage
{
FilmstripTile::FilmstripTile(TileView* parent) :
    AbstractTile(parent)
{
}

void FilmstripTile::render(QPainter& painter,
    const TileInfo& tileInfo,
    const QVariant& data)
{
    int w = painter.window().width();
    int h = painter.window().height();

    //   painter.drawLine(0,0,w-1,h-1);
    //   painter.drawLine(0,h,w-1,0);

    // TODO: Load fonts in advance??
    painter.setFont(QFont("helvetica", 18));

    if (!data.isNull())
    {
        if ((tileInfo.tileState & TileInfo::TileStateSelected) ==
            TileInfo::TileStateSelected)
            painter.setBrush(QBrush(QColor(Qt::darkGray).lighter(180),
                Qt::SolidPattern));
        else
            painter.setBrush(QBrush(QColor(Qt::darkGray),
                Qt::SolidPattern));
        // draw the tile
        painter.setPen(QColor(Qt::darkGray));
        painter.drawRect(0, 0, w - 2, h - 2);
        painter.setBrush(Qt::NoBrush);

        // draw the bevel
        painter.setPen(QColor(Qt::gray));
        // draw clock wise
        painter.drawLine(0, h - 1, 0, 0);     //left side
        painter.drawLine(0, 0, w - 1, 0);     // top side
        painter.setPen(QColor(Qt::black));
        painter.drawLine(w - 1, 0, w - 1, h - 1);    // right side
        painter.drawLine(w - 1, h - 1, 0, h - 1);     // bottom side

        Photo* info = data.value<Photo*>();

        // draw the id text
        painter.save();
        painter.setPen(QColor(Qt::darkGray).lighter(110));
        painter.setFont(QFont(QString("Verdana"), 24, QFont::Bold));
        int fontHeight = painter.fontMetrics().height();
        painter.drawText(5, fontHeight - 5, QString::number(info->id));

        painter.restore();

        // draw the image.
        QImage image = info->libraryPreviewsRGB();
        QRect  photoFinalDimension;

        if (!image.isNull())
        {
            float ratio = 0.90f;
            int   wf    = (int)(w * ratio);     // width frame
            int   hf    = (int)(h * ratio);     // height frame

            photoFinalDimension = fitFrame(image.size(), QSize(wf, hf));

            // move the frame to the center
            photoFinalDimension.translate((w - wf) / 2, (h - hf) / 2);
            painter.drawImage(photoFinalDimension, image);

            // draw border around image
            painter.setPen(QColor(Qt::black));
            painter.drawRect(photoFinalDimension);
        }
        else
        {
            // TODO: draw missing image indicator
            painter.setPen(QColor(Qt::red));
            painter.drawLine(w / 2 - 15, h / 2 - 15, w / 2 + 15,
                h / 2 + 15);
            painter.drawLine(w / 2 - 15, h / 2 + 15, w / 2 + 15,
                h / 2 - 15);
        }
    }
    else
    {
        // TODO: draw missing picture instead of blue cross
        painter.setPen(QColor(Qt::blue));
        painter.drawLine(0, 0, w - 1, h - 1);
        painter.drawLine(0, h - 1, w - 1, 0);
    }
}
}