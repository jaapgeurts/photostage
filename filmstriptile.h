#ifndef FILMSTRIPTILE_H
#define FILMSTRIPTILE_H

#include "widgets/abstracttile.h"

namespace PhotoStage
{
class FilmstripTile : public TileView::AbstractTile
{
    public:

        FilmstripTile(TileView::TileView* parent = 0);
        // AbstractCellRenderer interface
        void render(QPainter& painter,
            const TileView::TileInfo& tileInfo,
            const QVariant& data);

    private:
};
}
#endif // FILMSTRIPTILE_H
