#include "fixedlistview.h"

namespace Widgets
{
FixedListView::FixedListView(QWidget* parent) :
    QListView(parent)
{
    //    header()->close();
    // setViewport(NULL);
    //    viewport()->setFocusProxy(NULL);
}

void FixedListView::setModel(QAbstractItemModel* model)
{
    QListView::setModel(model);

    setMinimumHeight(CalculateHeight());
}

QSize FixedListView::sizeHint() const
{
    return QSize(width(), CalculateHeight());
}

int FixedListView::CalculateHeight() const
{
    int h = 0;

    int topLevelCount = model()->rowCount(rootIndex());

    for (int i = 0; i < topLevelCount; i++)
    {
        QModelIndex index = model()->index(i, 0, rootIndex());
        h += sizeHintForIndex(index).height() + 2;
    }

    return h;
}
}
