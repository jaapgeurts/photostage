#ifndef IMAGEDBCELLRENDERER_H
#define IMAGEDBCELLRENDERER_H

#include <QFont>
#include <QHash>

#include "widgets/abstracttile.h"
#include "engine/colortransform.h"

#define CHAR_STAR (0x2605)

class ImageDbTile : public AbstractTile
{
    Q_OBJECT

    public:

        ImageDbTile(TileView* parent = 0);

        // AbstractCellRenderer interface
        void render(QPainter &painter,const TileInfo &tileInfo, const QVariant &data);

        void mouseMoveEvent(QMouseEvent* event, const TileInfo &info);
        void mouseReleaseEvent(QMouseEvent* event, const TileInfo &info);

        void mouseEnterEvent(const TileInfo &info);
        void mouseLeaveEvent(const TileInfo &info);

    signals:

        //   void rotateLeftClicked(const QModelIndex & index);
        //   void rotateRightClicked(const QModelIndex & index);

        //    void ratingClicked(const QModelIndex &index, int rating);

    private:

        QRect resizeToFrameKeepAspectRatio(const QSize &src, const QSize &destFrame);

        QFont           mFontFoundIcons;
        // Hover states for rotate
        QHash<int,bool> mLeftHover;
        QHash<int,bool> mRightHover;
        QHash<int,int>  mRatingHover;
        ColorTransform  mColorTransform;
};

#endif // IMAGEDBCELLRENDERER_H
