#ifndef FIXEDTREEVIEW_H
#define FIXEDTREEVIEW_H

#include <QWidget>
#include <QTreeView>

#include "dndhandler.h"

namespace Widgets
{
class FixedTreeView : public QTreeView
{
    public:

        FixedTreeView(QWidget* parent = 0);

        void setModel(QAbstractItemModel* model);
        QSize sizeHint() const;

        void setDndHandler(DndHandler* handler);

    protected:

        void dragEnterEvent(QDragEnterEvent* event);
        void dragMoveEvent(QDragMoveEvent* event);
        void dropEvent(QDropEvent* event);

    private:

        DndHandler* mDndHandler;

        int CalculateHeight() const;
        int CalculateHeightRec(const QModelIndex& index) const;

        void onItemExpanded(const QModelIndex& index);
        void onItemCollapsed(const QModelIndex& index);
};
}

#endif // FIXEDTREEVIEW_H
