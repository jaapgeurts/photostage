#include <QDebug>
#include <QtSql>

#include "pathdao.h"

namespace PhotoStage
{
PathDAO* PathDAO::mInstance = NULL;

PathDAO* PathDAO::instance()
{
    if (mInstance == NULL)
        mInstance = new PathDAO();
    return mInstance;
}

PathDAO::PathDAO()
{
}

PathItem* PathDAO::createPathItems()
{
    // create table path (
    //    id integer primary key AUTOINCREMENT,
    //    directory varchar,
    //    parent integer)")))

    // first create the root item
    QSqlQuery query;

    QString   queryText = QString("select id, directory, parent_id from path where parent_id is NULL");

    if (!query.exec(queryText))
        qDebug() << query.lastError();

    PathItem* rootItem = NULL;

    if (query.first())
    {
        rootItem = new PathItem(query.value(0).toLongLong(),
                query.value(1).toString(),
                query.value(2).toLongLong());
        createPathItemsRec(rootItem);
    }
    return rootItem;
}

// TODO: convert this to lft/rgt tree function instead of recursive
void PathDAO::createPathItemsRec(PathItem* root)
{
    QSqlQuery query;
    QString   queryText = QString(
        "select p.id, p.directory, p.parent_id, count(f.id) \
        from path p left outer join photo f \
                     on p.id = f.path_id \
        where parent_id = :parent_id \
        group by p.id, p.directory, p.parent_id");

    query.prepare(queryText);

    query.bindValue(":parent_id", root->id);
    query.exec();

    //int total = 0;
    while (query.next())
    {
        PathItem* item = new PathItem(query.value(0).toLongLong(),
                query.value(1).toString(),
                query.value(1).toLongLong());
        item->count      = query.value(3).toInt();
        item->cumulative = item->count;
        item->parent     = root;
        root->children.append(item);
        createPathItemsRec(item);
        root->cumulative +=  item->cumulative;
    }
}

void PathDAO::deletePathItems(PathItem* root)
{
    PathItem* item;

    foreach(item, root->children)
    {
        deletePathItems(item);
    }
    root->children.clear();
    delete root;
}
}
