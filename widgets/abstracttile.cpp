#include "abstracttile.h"

#include "tileview.h"

AbstractTile::AbstractTile(TileView *parent) :QObject(parent)
{
}

void AbstractTile::update()
{
    (dynamic_cast<TileView*>(parent()))->update();
}
