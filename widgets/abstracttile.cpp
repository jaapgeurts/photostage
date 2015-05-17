#include "abstracttile.h"

#include "tileview.h"




AbstractTile::AbstractTile(TileView *parent)
{
    mParent = parent;
}

void AbstractTile::update()
{
    mParent->update();
}
