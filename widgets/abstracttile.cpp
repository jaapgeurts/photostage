#include "abstracttile.h"

#include "tileview.h"

namespace PhotoStage
{
    AbstractTile::AbstractTile(TileView* parent) : QObject(parent)
    {
    }

    void AbstractTile::update()
    {
        (dynamic_cast<TileView*>(parent()))->update();
    }
}