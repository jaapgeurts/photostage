#ifndef PHOTOSTAGE_COLLECTIONMODULE_H
#define PHOTOSTAGE_COLLECTIONMODULE_H

#include <QObject>
#include <QTreeView>

#include "librarymodule.h"
#include "widgets/fixedtreeview.h"
#include "collectionmodel.h"
#include "photomodel.h"

namespace PhotoStage
{
class CollectionModule : public LibraryModule
{
    friend class DropHandler;

    Q_OBJECT

    public:

        CollectionModule(QWidget* parent);

        void sourceChanged(PhotoModel::SourceType source);

    public slots:

        void onNewCollection();

    signals:

        void collectionSelected(long long id);

    private slots:

        void onFilesClicked(const QModelIndex& index);

    private:

        Widgets::FixedTreeView* mTrvwCollections;
        CollectionModel*        mCollectionModel;
};
}
#endif // PHOTOSTAGE_COLLECTIONMODULE_H
