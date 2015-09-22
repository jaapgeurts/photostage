#ifndef PHOTOSTAGE_FILMSTRIPTILE_H
#define PHOTOSTAGE_FILMSTRIPTILE_H

#include "widgets/abstracttile.h"

namespace PhotoStage
{
class FilmstripTile : public Widgets::AbstractTile
{
    public:

        FilmstripTile(Widgets::TileView* parent = 0);
        // AbstractCellRenderer interface
        void render(QPainter& painter, const Widgets::TileInfo& tileInfo, const QVariant& data);

        int modelRole() const;

    private:
};
}
#endif // PHOTOSTAGE_FILMSTRIPTILE_H
