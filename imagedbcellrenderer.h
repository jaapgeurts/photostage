#ifndef IMAGEDBCELLRENDERER_H
#define IMAGEDBCELLRENDERER_H

#include "widgets/abstractcellrenderer.h"


class ImageDbCellRenderer : public AbstractCellRenderer
{
public:
    ImageDbCellRenderer();

    // AbstractCellRenderer interface
    void render(QPainter &painter, const QVariant &data);

    void mouseReleaseEvent(QMouseEvent *event);

private:
    QRect resizeToFrameKeepAspectRatio(const QSize &src, const QSize &destFrame);
};

#endif // IMAGEDBCELLRENDERER_H
