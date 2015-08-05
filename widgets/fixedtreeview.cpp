#include <QHeaderView>
#include <QEvent>
#include <QDragMoveEvent>

#include <QDebug>

#include "fixedtreeview.h"

namespace Widgets
{
FixedTreeView::FixedTreeView(QWidget* parent) :
    QTreeView(parent)
{
    connect(this, &QTreeView::expanded, this, &FixedTreeView::onItemExpanded);
    connect(this, &QTreeView::collapsed, this, &FixedTreeView::onItemCollapsed);
    setAnimated(true);
    header()->close();
    // setViewport(NULL);
    //    viewport()->setFocusProxy(NULL);
}

void FixedTreeView::setModel(QAbstractItemModel* model)
{
    QTreeView::setModel(model);

    setMinimumHeight(CalculateHeight());
}

QSize FixedTreeView::sizeHint() const
{
    return QSize(width(), CalculateHeight());
}

int FixedTreeView::CalculateHeight() const
{
    int         h = 0;

    int         topLevelCount = model()->rowCount(rootIndex());

    QModelIndex index;

    for (int i = 0; i < topLevelCount; i++)
    {
        QModelIndex index = model()->index(i, 0, rootIndex());
        h += CalculateHeightRec(index);
    }

    if (h != 0)
    {
        h += header()->sizeHint().height();
        //        setMinimumHeight(h);
    }
    return h;
}

int FixedTreeView::CalculateHeightRec(const QModelIndex& index) const
{
    if (!index.isValid())
        return 0;

    if (!isExpanded(index))
    {
        return rowHeight(index);
    }

    int h          = sizeHintForIndex(index).height() + 2;
    int childCount = model()->rowCount(index);

    for (int i = 0; i < childCount; i++)
    {
        QModelIndex childIndex = model()->index(i, 0, index);
        h += CalculateHeightRec(childIndex);
    }

    return h;
}

void FixedTreeView::onItemExpanded(const QModelIndex& /*index*/)
{
    setMinimumHeight(CalculateHeight());
}

void FixedTreeView::onItemCollapsed(const QModelIndex& /*index*/)
{
    setMinimumHeight(CalculateHeight());
}
}
