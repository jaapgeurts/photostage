#ifndef IMAGEFILECELLRENDERER_H
#define IMAGEFILECELLRENDERER_H

#include "widgets/abstracttile.h"

class ImageFileTile : public AbstractTile
{
    Q_OBJECT

public:
    ImageFileTile(TileView* parent=0);

    void render(QPainter& painter, const TileInfo &tileInfo, const QVariant& data);

private:
    QRect resizeToFrameKeepAspectRatio(const QSize &src, const QSize &destFrame);

};

#endif // IMAGEFILECELLRENDERER_H
