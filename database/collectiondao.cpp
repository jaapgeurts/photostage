#include <QtSql>
#include <QSqlQuery>
#include <QDebug>

#include "dbutils.h"
#include "collectiondao.h"
#include "databaseaccess.h"
#include "collectionitem.h"

namespace PhotoStage
{
// All three collections (WORK = temporary collection, USER = user defined collection, IMPORT = import collections
// Each tree root node uses these respective names
static const QString SOURCE_NAME_USER   = "__USER";
static const QString SOURCE_NAME_WORK   = "__WORK";
static const QString SOURCE_NAME_IMPORT = "__IMPORT";

CollectionDAO::CollectionDAO(QObject* parent) :
    QObject(parent)
{
}

CollectionItem* CollectionDAO::getCollectionItems(CollectionSources sources)
{
    // first create the root item
    QSqlQuery q;
    QString   query = "select id, name, parent_id from collection where parent_id is NULL and ( 1=0 ";

    if (sources & UserSource)
        query += " or name = '" + SOURCE_NAME_USER + "'";

    if (sources & WorkSource)
        query += " or name = '" + SOURCE_NAME_WORK + "'";

    if (sources & ImportSource)
        query += " or name = '" + SOURCE_NAME_IMPORT + "'";

    query += " );";

    if (!q.exec(query))
        qDebug() << q.lastError();
    // qDebug() << q.lastQuery();

    CollectionItem* rootItem = new CollectionItem(-1, "ROOT ITEM", -1);

    while (q.next())
    {
        long long        id   = q.value(0).toLongLong();
        QString          name = q.value(1).toString();
        CollectionSource source;

        if (name == SOURCE_NAME_IMPORT)
            source = ImportSource;
        else if (name == SOURCE_NAME_WORK)
            source = WorkSource;
        else // (name == SOURCE_NAME_USER)
            source = UserSource;
        getCollectionItemsRec(rootItem, id, source);
    }
    return rootItem;
}

// TODO: convert this to lft/rgt tree function instead of recursive
void CollectionDAO::getCollectionItemsRec(CollectionItem* root, long long id, CollectionSource source)
{
    QSqlQuery query;
    QString   queryText = QString(
        "select c.id, c.name,c.parent_id, count(cf.photo_id) \
        from collection c left outer join collection_photo cf \
                     on c.id = cf.collection_id \
        where parent_id = :parent_id \
        group by c.id, c.name, c.parent_id \
        order by c.name;");

    query.prepare(queryText);

    query.bindValue(":parent_id", id);
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
        item->source     = source;
        root->children.append(item);
        getCollectionItemsRec(item, item->id, source);
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
            q.finish();
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
    q.finish();
    q.prepare("insert into collection (name,parent_id) values ( :name , :parent )");
    q.bindValue(":parent", parent);
    q.bindValue(":name", name);

    if (!q.exec())
        qDebug() << q.lastError();

    long long newid = q.lastInsertId().toLongLong();

    q.finish();

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

long long CollectionDAO::addImportCollection(int /*amount*/)
{
    QDateTime dt = QDateTime::currentDateTime();

    QString   name =
        tr("Import %1").arg(QLocale::system().toString(dt, QLocale::ShortFormat));

    return addCollectionInternal(nullptr, SOURCE_NAME_IMPORT, name);
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
