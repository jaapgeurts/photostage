#ifndef PHOTOSTAGE_FIXEDLISTVIEW_H
#define PHOTOSTAGE_FIXEDLISTVIEW_H

#include <QListView>

namespace Widgets {
class FixedListView : public QListView
{
public:
  FixedListView(QWidget* parent = 0);
  void  setModel(QAbstractItemModel* model);
  QSize sizeHint() const;

private slots:

  void onModelReset();
  void onRowsAddedDeleted(const QModelIndex&, int, int);

private:
  int CalculateHeight() const;
};
} // namespace Widgets
#endif // FIXEDLISTVIEW_H
