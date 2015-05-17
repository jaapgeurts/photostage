#ifndef IMAGEDBCELLRENDERER_H
#define IMAGEDBCELLRENDERER_H

#include <QFont>
#include <QHash>

#include "widgets/abstracttile.h"


class ImageDbTile : public AbstractTile
{
public:
    ImageDbTile(TileView* parent=0);

    // AbstractCellRenderer interface
    void render(QPainter &painter, TileInfo &tileInfo, const QVariant &data);

    bool mouseMoveEvent(QMouseEvent *event, TileInfo &info);

private:
    QRect resizeToFrameKeepAspectRatio(const QSize &src, const QSize &destFrame);
    QFont mFontAwesome;
    QHash<int,bool> mLeftHover;
};

#endif // IMAGEDBCELLRENDERER_H
