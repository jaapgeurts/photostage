#ifndef PHOTOSTAGE_SHORTCUTMODULE_H
#define PHOTOSTAGE_SHORTCUTMODULE_H

#include "librarymodule.h"
#include "database/databaseaccess.h"
#include "collectionmodel.h"
#include "widgets/fixedlistview.h"
#include "photomodel.h"

namespace PhotoStage
{
class ShortcutModule : public LibraryModule
{
    Q_OBJECT

    public:

        explicit ShortcutModule(QWidget* parent = 0);

        void sourceChanged(PhotoModel::SourceType source);

    signals:

        void workCollectionSelected(long long id);
        void importCollectionSelected(long long id);

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
