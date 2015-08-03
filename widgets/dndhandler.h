#ifndef  TILEVIEW_TILEVIEWDNDHANDLER_H
#define TILEVIEW_TILEVIEWDNDHANDLER_H

#include <QModelIndex>
#include <QMimeData>
#include <QPixMap>

#include <QDragEnterEvent>

namespace TileView
{
class DndHandler
{
    public:

        virtual bool dragStart(const QModelIndex& index,
            Qt::DropActions& action,
            QMimeData* mimeData,
            QPixmap& image,
            QPoint& hotspot)                           = 0;
        virtual void dragEnter(QDragEnterEvent* event) = 0;
        virtual void dragLeave(QDragLeaveEvent* event) = 0;
        virtual void dragOver(QDragMoveEvent* event)   = 0;
        virtual void dragDrop(QDropEvent* event)       = 0;
};
}
#endif //  TILEVIEW_TILEVIEWDNDHANDLER_H
