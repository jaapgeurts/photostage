#ifndef WIDGETS_FIXEDTREEVIEW_H
#define WIDGETS_FIXEDTREEVIEW_H

#include <QWidget>
#include <QTreeView>

namespace Widgets
{
class FixedTreeView : public QTreeView
{
    public:

        FixedTreeView(QWidget* parent = 0);

        void setModel(QAbstractItemModel* model);
        QSize sizeHint() const;

    private slots:

        void onItemExpanded(const QModelIndex& index);
        void onItemCollapsed(const QModelIndex& index);

        void onRowsAddedDeleted(const QModelIndex& index, int first, int last);
        void onModelReset();

    private:

        int CalculateHeight() const;
        int CalculateHeightRec(const QModelIndex& index) const;
};
}

#endif // FIXEDTREEVIEW_H
