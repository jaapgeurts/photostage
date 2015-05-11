#ifndef IMAGEFILECELLRENDERER_H
#define IMAGEFILECELLRENDERER_H

#include "abstractcellrenderer.h"

class ImageFileCellRenderer : public AbstractCellRenderer
{
public:
    ImageFileCellRenderer();

    void render(QPainter& painter, const QVariant& data);
};

#endif // IMAGEFILECELLRENDERER_H
