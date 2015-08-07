#ifndef PHOTOSTAGE_COLLECTIONDAO_H
#define PHOTOSTAGE_COLLECTIONDAO_H

#include <QObject>

#include "nullable.h"
#include "photo.h"
#include "collectionitem.h"

namespace PhotoStage
{
class DatabaseAccess;

class CollectionDAO : public QObject
{
    friend class DatabaseAccess;

    Q_OBJECT

    public:

        CollectionItem* getCollectionItems();

        void deleteCollectionItems(CollectionItem* root);
        void addPhotosToCollection(long long collectionId, const QList<long long>& photoIds);
        void addCollection(const Nullable<long long>& parentid, const QString& name);
        void removePhotosFromCollection(long long collectionid, const QList<Photo>& list);

        //    Nullable<long long> collectionIdForPhoto(const Photo& photo) const;

    signals:

        void collectionAdded();
        void collectionsChanged();
        void photosRemoved(long long collectionid, const QList<Photo>& list);

        //        void collectionDeleted();

    private:

        CollectionDAO(QObject* parent = 0);

        void getCollectionItemsRec(CollectionItem* root);
        long long rebuildCollectionTree(long long parent_id, long long left);
};
}
#endif // PHOTOSTAGE_COLLECTIONDAO_H
