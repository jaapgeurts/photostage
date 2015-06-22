#ifndef COLLECTIONMODULE_H
#define COLLECTIONMODULE_H

#include <QObject>
#include <QTreeView>

#include "librarymodule.h"

namespace PhotoStage
{
    class CollectionModule : public LibraryModule
    {
        Q_OBJECT

        public:

            CollectionModule(QWidget* parent);

        private:

            QTreeView* mTrvwCollections;
    };
}
#endif // COLLECTIONMODULE_H