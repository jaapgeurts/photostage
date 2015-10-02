#ifndef PHOTO_HISTORYMODULE_H
#define PHOTO_HISTORYMODULE_H

#include <QWidget>

#include "widgets/fixedlistview.h"
#include "historymodel.h"
#include "developmodule.h"

namespace PhotoStage
{
class HistoryModule : public DevelopModule
{
    public:

        HistoryModule(QWidget* parent = 0l);

        virtual void setPhoto(Photo photo);

    private slots:

        void onListClicked(const QModelIndex& index);

    private:

        Widgets::FixedListView* mLvHistory;
        HistoryModel*           mHistoryModel;
};
}

#endif // PHOTO_HISTORYMODULE_H
