#ifndef PHOTOSTAGE_COLLECTIONMODULE_H
#define PHOTOSTAGE_COLLECTIONMODULE_H

#include <QObject>
#include <QTreeView>

#include "librarymodule.h"
#include "widgets/fixedtreeview.h"
#include "collectionmodel.h"

namespace PhotoStage
{
class DropHandler;

class CollectionModule : public LibraryModule
{
    friend class DropHandler;

    Q_OBJECT

    public:

        CollectionModule(QWidget* parent);

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
