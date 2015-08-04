#ifndef PHOTOSTAGE_COLLECTIONDAO_H
#define PHOTOSTAGE_COLLECTIONDAO_H

#include "collectionitem.h"

namespace PhotoStage
{
class CollectionDAO
{
    public:

        static CollectionDAO* instance();

        CollectionDAO();
        CollectionItem* createCollectionItems();

        void deleteCollectionItems(CollectionItem* root);

    private:

        static CollectionDAO* mInstance;

        void createCollectionItemsRec(CollectionItem* root);
};
}
#endif // PHOTOSTAGE_COLLECTIONDAO_H
