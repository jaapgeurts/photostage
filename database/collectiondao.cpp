#include <QtSql>
#include <QSqlQuery>
#include <QDebug>

#include "dbutils.h"
#include "collectiondao.h"
#include "databaseaccess.h"

namespace PhotoStage
{
static const QString SOURCE_NAME_USER   = "USER";
static const QString SOURCE_NAME_WORK   = "WORK";
static const QString SOURCE_NAME_IMPORT = "IMPORT";

CollectionDAO::CollectionDAO(QObject* parent) :
    QObject(parent)
{
}

CollectionItem* CollectionDAO::getCollectionItems(CollectionSource source)
{
    // first create the root item
    QSqlQuery query;
    QString   src;

    switch (source)
    {
        case UserSource:
            src = SOURCE_NAME_USER;
            break;

        case WorkSource:
            src = SOURCE_NAME_WORK;
            break;

        case ImportSource:
            src = SOURCE_NAME_IMPORT;
            break;
    }

    query.prepare("select id, name, parent_id from collection where parent_id is NULL and name = :name;");
    query.bindValue(":name", src);

    if (!query.exec())
        qDebug() << query.lastError();

    CollectionItem* rootItem = NULL;

    if (query.first())
    {
        rootItem = new CollectionItem(query.value(0).toLongLong(),
                query.value(1).toString(),
                query.value(2).toLongLong());

        getCollectionItemsRec(rootItem);
    }
    return rootItem;
}

// TODO: convert this to lft/rgt tree function instead of recursive
void CollectionDAO::getCollectionItemsRec(CollectionItem* root)
{
    QSqlQuery query;
    QString   queryText = QString(
        "select c.id, c.name,c.parent_id, count(cf.photo_id) \
        from collection c left outer join collection_photo cf \
                     on c.id = cf.collection_id \
        where parent_id = :parent_id \
        group by c.id, c.name, c.parent_id");

    query.prepare(queryText);

    query.bindValue(":parent_id", root->id);
    query.exec();

    //int total = 0;
    while (query.next())
    {
        CollectionItem* item = new CollectionItem(query.value(0).toLongLong(),
                query.value(1).toString(),
                query.value(1).toLongLong());
        item->count      = query.value(3).toInt();
        item->cumulative = item->count;
        item->parent     = root;
        root->children.append(item);
        getCollectionItemsRec(item);
        root->cumulative +=  item->cumulative;
    }
}

long long CollectionDAO::addCollectionInternal(const Nullable<long long>& parentid,
    const QString& rootname,
    const QString& name)
{
    long long parent;
    QSqlQuery q;

    if (parentid == nullptr)
    {
        // add as a root item. make sure there is a sentinel root item first named USER.
        // These are the user created collections
        // get the root id ( a single root is needed )
        q.clear();
        q.prepare("select id from collection where parent_id is NULL and name = :name;");
        q.bindValue(":name", rootname);
        q.exec();

        if (q.first())
        {
            parent = q.value(0).toLongLong();
        }
        else
        {
            // insert root because there is none.
            q.clear();
            q.prepare("insert into collection (name,parent_id) values(:name,NULL) ");
            q.bindValue(":name", rootname);
            q.exec();
            parent = q.lastInsertId().toLongLong();
        }
    }
    else
    {
        parent = *parentid;
    }

    // insert the collection.
    q.clear();
    q.prepare("insert into collection (name,parent_id) values ( :name , :parent )");
    q.bindValue(":parent", parent);
    q.bindValue(":name", name);

    if (!q.exec())
        qDebug() << q.lastError();

    long long newid = q.lastInsertId().toLongLong();

    // TODO: improve performance and don't rebuild tree on each insert.
    rebuildCollectionTree(parent, 1);

    emit collectionAdded(newid);

    return newid;
}

void CollectionDAO::deleteCollectionItems(CollectionItem* root)
{
    CollectionItem* item;

    foreach(item, root->children)
    {
        deleteCollectionItems(item);
    }
    root->children.clear();
    delete root;
}

void CollectionDAO::addPhotosToCollection(long long collectionId, const QList<long long>& photoIds)
{
    // TODO: wrap this in transaction
    DatabaseAccess::instance()->beginTransaction();
    QSqlQuery q;
    QString   query("insert into collection_photo (collection_id,photo_id) values (:id,:photo_id);");

    q.prepare(query);
    q.bindValue(":id", collectionId);

    foreach(long long id, photoIds)
    {
        q.bindValue(":photo_id", id);

        if (!q.exec())
        {
            qDebug() << q.lastError();
            qDebug() << q.lastQuery();
        }
    }
    DatabaseAccess::instance()->endTransaction();
    emit collectionsChanged();
}

long long CollectionDAO::addCollection(const Nullable<long long>& parentid, const QString& name)
{
    return addCollectionInternal(parentid, SOURCE_NAME_USER, name);
}

long long CollectionDAO::addImportCollection()
{
    QDateTime dt = QDateTime::currentDateTime();

    return addCollectionInternal(nullptr, SOURCE_NAME_IMPORT, dt.toString());
}

long long CollectionDAO::addWorkCollection(const QString& name)
{
    return addCollectionInternal(nullptr, SOURCE_NAME_WORK, name);
}

void CollectionDAO::removePhotosFromCollection(long long collectionid, const QList<Photo>& list)
{
    QSqlQuery q;

    QString   query = "delete from collection_photo where collection_id = :cid and photo_id in (:photoids);";

    QString   photoids = joinIds(list);

    query.replace(":photoids", photoids);
    q.prepare(query);
    q.bindValue(":cid", collectionid);

    if (!q.exec())
    {
        qDebug() << q.lastError();
        qDebug() << q.lastQuery();
    }

    emit photosRemoved(collectionid, list);
}

long long CollectionDAO::rebuildCollectionTree(long long parent_id, long long left)
{
    // the right value of this node is the left value + 1

    long long right = left + 1;

    // get all children of this node

    QSqlQuery q;

    q.prepare("select id from collection where parent_id = :parent_id;");
    q.bindValue(":parent_id", parent_id);

    q.exec();

    while (q.next())
    {
        long long id = q.value(0).toLongLong();
        right = rebuildCollectionTree(id, right);
    }

    q.prepare("update collection set lft=:left, rgt=:right where id=:parent_id;");

    q.bindValue(":left", left);
    q.bindValue(":right", right);
    q.bindValue(":parent_id", parent_id);

    q.exec();

    // return the right value of this node + 1

    return right + 1;
}
}
