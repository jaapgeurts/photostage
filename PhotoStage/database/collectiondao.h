#ifndef PHOTOSTAGE_COLLECTIONDAO_H
#define PHOTOSTAGE_COLLECTIONDAO_H

#include <QObject>

#include "nullable.h"
#include "photo.h"

namespace PhotoStage
{
class DatabaseAccess;
struct CollectionItem;

class CollectionDAO : public QObject
{
    friend class DatabaseAccess;

    Q_OBJECT

    public:

        enum CollectionSource
        {
            UserSource   = 0x01,
            WorkSource   = 0x02,
            ImportSource = 0x04
        };
        Q_DECLARE_FLAGS(CollectionSources, CollectionSource)

        CollectionItem * getCollectionItems(CollectionSources source);

        void deleteCollectionItems(CollectionItem* root);
        void addPhotosToCollection(long long collectionId, const QList<long long>& photoIds);

        long long addCollection(const Nullable<long long>& parentid, const QString& name);
        long long addImportCollection(int);
        long long addWorkCollection(const QString& name);

        //    Nullable<long long> collectionIdForPhoto(const Photo& photo) const;
        void removePhotosFromCollection(long long collectionid, const QList<Photo>& list);

    signals:

        void collectionAdded(long long id);
        void collectionsChanged();
        void photosRemoved(long long collectionid, const QList<Photo>& list);

        //        void collectionDeleted();

    private:

        CollectionDAO(QObject* parent = 0);

        void getCollectionItemsRec(CollectionItem* root, long long id, CollectionSource source);
        long long rebuildCollectionTree(long long parent_id, long long left);

        long long addCollectionInternal(const Nullable<long long>& parentid,
            const QString& rootname,
            const QString& name);
};
}

Q_DECLARE_OPERATORS_FOR_FLAGS(PhotoStage::CollectionDAO::CollectionSources)

#endif // PHOTOSTAGE_COLLECTIONDAO_H
