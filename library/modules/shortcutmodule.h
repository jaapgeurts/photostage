#ifndef PHOTOSTAGE_SHORTCUTMODULE_H
#define PHOTOSTAGE_SHORTCUTMODULE_H

#include "librarymodule.h"
#include "database/databaseaccess.h"
#include "collectionmodel.h"
#include "widgets/fixedlistview.h"

namespace PhotoStage
{
class ShortcutModule : public LibraryModule
{
    Q_OBJECT

    public:

        explicit ShortcutModule(QWidget* parent = 0);

    signals:

        void collectionSelected(long long id);

    private slots:

        void onWorkListClicked(const QModelIndex& index);
        void onImportListClicked(const QModelIndex& index);

    private:

        Widgets::FixedListView* mLvWorkList;
        Widgets::FixedListView* mLvImportList;
        CollectionModel*        mWorkModel;
        CollectionModel*        mImportModel;
};
}
#endif // SHORTCUTMODULE_H
