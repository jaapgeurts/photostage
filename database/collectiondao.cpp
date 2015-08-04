#include <QtSql>
#include <QSqlQuery>
#include <QDebug>

#include "collectiondao.h"

namespace PhotoStage
{
CollectionDAO* CollectionDAO::mInstance = NULL;

CollectionDAO* CollectionDAO::instance()
{
    if (mInstance == NULL)
        mInstance = new CollectionDAO();
    return mInstance;
}

CollectionDAO::CollectionDAO()
{
}

CollectionItem* CollectionDAO::createCollectionItems()
{
    // first create the root item
    QSqlQuery query;

    QString   queryText = QString("select id, name, parent_id from collection where parent_id is NULL");

    if (!query.exec(queryText))
        qDebug() << query.lastError();

    CollectionItem* rootItem = NULL;

    if (query.first())
    {
        rootItem = new CollectionItem(query.value(0).toLongLong(),
                query.value(1).toString(),
                query.value(2).toLongLong());

        createCollectionItemsRec(rootItem);
    }
    return rootItem;
}

// TODO: convert this to lft/rgt tree function instead of recursive
void CollectionDAO::createCollectionItemsRec(CollectionItem* root)
{
    QSqlQuery query;
    QString   queryText = QString(
        "select p.id, p.name, p.parent_id, count(f.id) \
        from collection p left outer join photo f \
                     on p.id = f.path_id \
        where parent_id = :parent_id \
        group by p.id, p.name, p.parent_id");

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
        createCollectionItemsRec(item);
        root->cumulative +=  item->cumulative;
    }
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
}
