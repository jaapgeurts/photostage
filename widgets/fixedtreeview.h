#ifndef FIXEDTREEVIEW_H
#define FIXEDTREEVIEW_H

#include <QWidget>
#include <QTreeView>

class FixedTreeView : public QTreeView
{
public:
    FixedTreeView(QWidget *parent =0);
    void setModel(QAbstractItemModel *model);
    QSize sizeHint() const;

private:
    int CalculateHeight() const;
    int CalculateHeightRec(const QModelIndex & index) const;

    void onItemExpanded(const QModelIndex & index);
    void onItemCollapsed(const QModelIndex & index);
};

#endif // FIXEDTREEVIEW_H
