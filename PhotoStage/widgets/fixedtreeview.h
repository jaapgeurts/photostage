#ifndef WIDGETS_FIXEDTREEVIEW_H
#define WIDGETS_FIXEDTREEVIEW_H

#include <QTreeView>
#include <QWidget>

namespace Widgets {
class FixedTreeView : public QTreeView
{
public:
  FixedTreeView(QWidget* parent = 0);

  void  setModel(QAbstractItemModel* model);
  QSize sizeHint() const;

private slots:

  void onItemExpanded(const QModelIndex& index);
  void onItemCollapsed(const QModelIndex& index);

  void onRowsAddedDeleted(const QModelIndex&, int, int);
  void onModelReset();

private:
  int CalculateHeight() const;
  int CalculateHeightRec(const QModelIndex& index) const;
};
} // namespace Widgets

#endif // FIXEDTREEVIEW_H
