#ifndef FILMSTRIPTILE_H
#define FILMSTRIPTILE_H

#include "widgets/abstracttile.h"

namespace PhotoStage
{
    class FilmstripTile : public AbstractTile
    {
        public:

            FilmstripTile(TileView* parent = 0);
            // AbstractCellRenderer interface
            void render(QPainter& painter,
                const TileInfo& tileInfo,
                const QVariant& data);

        private:
    };
}
#endif // FILMSTRIPTILE_H