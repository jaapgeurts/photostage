#include "abstracttile.h"

#include "tileview.h"

namespace Widgets
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
