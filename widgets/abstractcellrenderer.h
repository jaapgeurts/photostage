#ifndef ABSTRACTCELLRENDERER_H
#define ABSTRACTCELLRENDERER_H

#include <QPainter>
#include <QVariant>
#include <QMouseEvent>

class AbstractCellRenderer
{
public:
    virtual void render(QPainter& painter, const QVariant& data) = 0;
    virtual void mouseReleaseEvent(QMouseEvent*event) {};
};

#endif // ABSTRACTCELLRENDERER_H
