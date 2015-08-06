#ifndef PHOTOSTAGE_COLLECTIONDAO_H
#define PHOTOSTAGE_COLLECTIONDAO_H

#include "nullable.h"

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
        void addPhotosToCollection(long long collectionId, const QList<long long>& photoIds);
        void addCollection(const Nullable<long long>& parentid, const QString& name);

    private:

        static CollectionDAO* mInstance;

        void createCollectionItemsRec(CollectionItem* root);
        long long rebuildCollectionTree(long long parent_id, long long left);
};
}
#endif // PHOTOSTAGE_COLLECTIONDAO_H
