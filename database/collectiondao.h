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

        enum CollectionSource
        {
            UserSource = 1,
            WorkSource,
            ImportSource
        };

        CollectionItem* getCollectionItems(CollectionSource source);

        void deleteCollectionItems(CollectionItem* root);
        void addPhotosToCollection(long long collectionId, const QList<long long>& photoIds);

        long long addCollection(const Nullable<long long>& parentid, const QString& name);
        long long addImportCollection();
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

        void getCollectionItemsRec(CollectionItem* root);
        long long rebuildCollectionTree(long long parent_id, long long left);

        long long addCollectionInternal(const Nullable<long long>& parentid,
            const QString& rootname,
            const QString& name);
};
}
#endif // PHOTOSTAGE_COLLECTIONDAO_H
