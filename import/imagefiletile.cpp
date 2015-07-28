#include <QDir>

#include "imagefiletile.h"
#include "previewinfo.h"
#include "utils.h"

namespace PhotoStage
{
ImageFileTile::ImageFileTile(TileView::TileView* parent) : AbstractTile(parent)
{
}

void ImageFileTile::render(QPainter& painter,
    const TileView::TileInfo& tileInfo,
    const QVariant& data)
{
    int w = painter.window().width();
    int h = painter.window().height();

    //   painter.drawLine(0,0,w-1,h-1);
    //   painter.drawLine(0,h,w-1,0);

    // draw the selection background
    if ((tileInfo.tileState & TileView::TileInfo::TileStateSelected) ==
        TileView::TileInfo::TileStateSelected)
        painter.setBrush(QBrush(QColor(Qt::darkGray).lighter(180),
            Qt::SolidPattern));
    else
        painter.setBrush(QBrush(QColor(Qt::darkGray), Qt::SolidPattern));
    // draw the tile
    painter.setPen(QColor(Qt::darkGray));
    painter.drawRect(1, 1, w - 2, h - 2);
    painter.setBrush(Qt::NoBrush);

    // draw the bevel of the tile
    painter.setPen(QColor(Qt::gray));
    // draw clock wise
    painter.drawLine(0, h - 1, 0, 0);     //left side
    painter.drawLine(0, 0, w - 1, 0);     // top side
    painter.setPen(QColor(Qt::black));
    painter.drawLine(w - 1, 0, w - 1, h - 1);    // right side
    painter.drawLine(w - 1, h - 1, 0, h - 1);     // bottom side

    PreviewInfo info;

    if (!data.isNull())
    {
        info = data.value<PreviewInfo>();
        QImage image = info.image;

        if (!image.isNull())
        {
            float ratio = 0.70f;
            int   wf    = (int)(w * ratio);     // width frame
            int   hf    = (int)(h * ratio);     // height frame

            QRect newSize = fitFrame(image.size(), QSize(wf, hf));

            // move the frame to the center
            newSize.translate((w - wf) / 2, (h - hf) / 2);
            // and draw the image
            painter.drawImage(newSize, image);

            // draw border around image
            painter.setPen(QColor(Qt::black));
            painter.drawRect(newSize);

            // draw the file name
            QFont fileNameFont = QFont("helvetica", 10);
            painter.setFont(fileNameFont);
            painter.setPen(QColor(Qt::lightGray));
            QString name =
                info.filePath.mid(info.filePath.lastIndexOf(
                        QDir::separator()) + 1);
            QFontMetrics m(fileNameFont);
            int          textWidth = m.width(name);
            painter.drawText((w - textWidth) / 2, h - 5, name);

            if (info.isInLibrary)
            {
                painter.fillRect(0, 0, w, h, QBrush(QColor(0, 0, 0, 80)));
            }
        }
        else
        {
            // invalid image
            painter.setPen(QColor(Qt::red));
            painter.drawLine(0, 0, w - 1, h - 1);
            painter.drawLine(0, h - 1, w - 1, 0);
        }
    }
    else
    {
        // TODO: draw missing picture
        painter.setPen(QColor(Qt::blue));
        painter.drawLine(0, 0, w - 1, h - 1);
        painter.drawLine(0, h - 1, w - 1, 0);
    }
}
}
