#ifndef COLLECTIONMODULE_H
#define COLLECTIONMODULE_H

#include <QObject>
#include <QTreeView>

#include "librarymodule.h"
#include "widgets/fixedtreeview.h"
#include "collectionmodel.h"

namespace PhotoStage
{
class CollectionModule : public LibraryModule
{
    Q_OBJECT

    public:

        CollectionModule(QWidget* parent);

    private:

        FixedTreeView*   mTrvwCollections;
        CollectionModel* mCollectionModel;
};
}
#endif // COLLECTIONMODULE_H
