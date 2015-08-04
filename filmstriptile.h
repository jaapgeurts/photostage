#ifndef FILMSTRIPTILE_H
#define FILMSTRIPTILE_H

#include "widgets/abstracttile.h"

namespace PhotoStage
{
class FilmstripTile : public Widgets::AbstractTile
{
    public:

        FilmstripTile(Widgets::TileView* parent = 0);
        // AbstractCellRenderer interface
        void render(QPainter& painter, const Widgets::TileInfo& tileInfo, const QVariant& data);

    private:
};
}
#endif // FILMSTRIPTILE_H
