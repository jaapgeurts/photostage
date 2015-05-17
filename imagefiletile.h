#ifndef IMAGEFILECELLRENDERER_H
#define IMAGEFILECELLRENDERER_H

#include "widgets/abstracttile.h"

class ImageFileTile : public AbstractTile
{
    Q_OBJECT

public:
    ImageFileTile(TileView* parent=0);

    void render(QPainter& painter, const TileInfo &info, const QVariant& data);
};

#endif // IMAGEFILECELLRENDERER_H
