#ifndef FILMSTRIPTILE_H
#define FILMSTRIPTILE_H

#include "widgets/abstracttile.h"
#include "engine/colortransform.h"

class FilmstripTile : public AbstractTile
{
    public:

        FilmstripTile(TileView* parent = 0);
        // AbstractCellRenderer interface
        void render(QPainter &painter,const TileInfo &tileInfo, const QVariant &data);

    private:

        QRect resizeToFrameKeepAspectRatio(const QSize &src, const QSize &destFrame);

        ColorTransform mColorTransform;
};

#endif // FILMSTRIPTILE_H
