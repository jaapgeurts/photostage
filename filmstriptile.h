#ifndef FILMSTRIPTILE_H
#define FILMSTRIPTILE_H

#include "widgets/abstracttile.h"

class FilmstripTile : public AbstractTile
{
public:
    FilmstripTile(TileView* parent=0);

    // AbstractCellRenderer interface
    void render(QPainter &painter,const TileInfo &tileInfo, const QVariant &data);

private:
    QRect resizeToFrameKeepAspectRatio(const QSize &src, const QSize &destFrame);
};

#endif // FILMSTRIPTILE_H
