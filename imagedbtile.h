#ifndef IMAGEDBCELLRENDERER_H
#define IMAGEDBCELLRENDERER_H

#include <QFont>
#include <QHash>

#include "widgets/abstracttile.h"


class ImageDbTile : public AbstractTile
{
    Q_OBJECT

public:
    ImageDbTile(TileView* parent=0);

    // AbstractCellRenderer interface
    void render(QPainter &painter,const TileInfo &tileInfo, const QVariant &data);

    void mouseMoveEvent(QMouseEvent *event, const TileInfo &info);
    void mouseReleaseEvent(QMouseEvent *event,const TileInfo &info);

signals:

    void rotateLeftClicked(const QModelIndex & index);
    void rotateRightClicked(const QModelIndex & index);

private:
    QRect resizeToFrameKeepAspectRatio(const QSize &src, const QSize &destFrame);
    QFont mFontAwesome;
    // Hover states for rotate
    QHash<int,bool> mLeftHover;
    QHash<int,bool> mRightHover;
};

#endif // IMAGEDBCELLRENDERER_H
