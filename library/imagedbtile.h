#ifndef IMAGEDBCELLRENDERER_H
#define IMAGEDBCELLRENDERER_H

#include <QFont>
#include <QHash>

#include "widgets/abstracttile.h"

#define CHAR_STAR (0x2605)

namespace PhotoStage
{
class ImageDbTile : public TileView::AbstractTile
{
    Q_OBJECT

    public:

        ImageDbTile(TileView::TileView* parent = 0);

        // AbstractCellRenderer interface
        void render(QPainter& painter,
            const TileView::TileInfo& tileInfo,
            const QVariant& data);

        void mouseMoveEvent(QMouseEvent* event, const TileView::TileInfo& info);
        void mouseReleaseEvent(QMouseEvent* event, const TileView::TileInfo& info);

        void mouseEnterEvent(const TileView::TileInfo& info);
        void mouseLeaveEvent(const TileView::TileInfo& info);

    signals:

        //   void rotateLeftClicked(const QModelIndex & index);
        //   void rotateRightClicked(const QModelIndex & index);

        //    void ratingClicked(const QModelIndex &index, int rating);

    private:

        const float      IMAGE_TO_TILE_RATIO = 0.80f;

        QFont            mFontGeneralFoundIcons;
        // Hover states for rotate
        QHash<int, bool> mLeftHover;
        QHash<int, bool> mRightHover;
        QHash<int, int>  mRatingHover;
};
}

#endif // IMAGEDBCELLRENDERER_H
