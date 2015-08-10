#ifndef PHOTOSTAGE_FIXEDLISTVIEW_H
#define PHOTOSTAGE_FIXEDLISTVIEW_H

#include <QListView>

namespace Widgets
{
class FixedListView : public QListView
{
    public:

        FixedListView(QWidget* parent = 0);
        void setModel(QAbstractItemModel* model);
        QSize sizeHint() const;

    private:

        int CalculateHeight() const;
};
}
#endif // FIXEDLISTVIEW_H
