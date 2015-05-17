#ifndef IMAGEFILECELLRENDERER_H
#define IMAGEFILECELLRENDERER_H

#include "widgets/abstracttile.h"

class ImageFileTile : public AbstractTile
{
public:
    ImageFileTile(TileView* parent=0);

    void render(QPainter& painter, TileInfo &info, const QVariant& data);
};

#endif // IMAGEFILECELLRENDERER_H
