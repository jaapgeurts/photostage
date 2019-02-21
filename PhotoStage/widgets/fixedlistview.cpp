#include "fixedlistview.h"

namespace Widgets {
FixedListView::FixedListView(QWidget* parent) : QListView(parent)
{
  //    header()->close();
  // setViewport(NULL);
  //    viewport()->setFocusProxy(NULL);
}

void FixedListView::setModel(QAbstractItemModel* model)
{
  QListView::setModel(model);

  setMinimumHeight(CalculateHeight());

  connect(model, &QAbstractItemModel::modelReset, this,
          &FixedListView::onModelReset);
  connect(model, &QAbstractItemModel::rowsInserted, this,
          &FixedListView::onRowsAddedDeleted);
  connect(model, &QAbstractItemModel::rowsRemoved, this,
          &FixedListView::onRowsAddedDeleted);
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

void FixedListView::onRowsAddedDeleted(const QModelIndex& /*index*/,
                                       int /*first*/, int /*last*/)
{
  setMinimumHeight(CalculateHeight());
}

void FixedListView::onModelReset()
{
  setMinimumHeight(CalculateHeight());
}
} // namespace Widgets
