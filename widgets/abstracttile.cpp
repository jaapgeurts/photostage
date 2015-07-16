#include "abstracttile.h"

#include "tileview.h"

namespace TileView
{
AbstractTile::AbstractTile(TileView* parent) : QObject(parent)
{
}

void AbstractTile::update()
{
    (dynamic_cast<TileView*>(parent()))->update();
}

TileInfo::TileInfo() :
    tileState(TileStateNone)
{
}
}
