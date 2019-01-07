#include "abstracttile.h"

#include "tileview.h"

namespace Widgets
{
AbstractTile::AbstractTile(TileView* parent) : QObject(parent)
{
}

AbstractTile::~AbstractTile()
{
}

void AbstractTile::setSize(const QSize& size)
{
    mSize = size;
}

QSize AbstractTile::size() const
{
    return mSize;
}

int AbstractTile::modelRole() const
{
    return ImageRole;
}

void AbstractTile::mouseMoveEvent(QMouseEvent* e, const TileInfo&)
{
    e->ignore();
}

void AbstractTile::mousePressEvent(QMouseEvent* e, const TileInfo&)
{
    e->ignore();
}

void AbstractTile::mouseEnterEvent(const TileInfo&)
{
}

void AbstractTile::mouseLeaveEvent(const TileInfo&)
{
}

void AbstractTile::mouseReleaseEvent(QMouseEvent* e, const TileInfo&)
{
    e->ignore();
}

void AbstractTile::update()
{
    (dynamic_cast<TileView*>(parent()))->update();
}

TileInfo::TileInfo() : tileState(TileStateNone)
{
}
}
