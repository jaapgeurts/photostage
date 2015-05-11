#ifndef ABSTRACTCELLRENDERER_H
#define ABSTRACTCELLRENDERER_H

#include <QPainter>
#include <QVariant>

class AbstractCellRenderer
{
public:
    virtual void render(QPainter& painter, const QVariant& data) = 0;
};

#endif // ABSTRACTCELLRENDERER_H
