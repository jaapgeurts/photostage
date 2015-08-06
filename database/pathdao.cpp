#include <QDebug>

#include "pathdao.h"

namespace PhotoStage
{
PathDAO::PathDAO(QObject* parent) :
    QObject(parent)
{
}

PathItem* PathDAO::getPathItems()
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
        getPathItemsRec(rootItem);
    }
    return rootItem;
}

// TODO: convert this to lft/rgt tree function instead of recursive
void PathDAO::getPathItemsRec(PathItem* root)
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
        getPathItemsRec(item);
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

// This function will check the path hierarchy in the database and insert the necessary paths
// it returns the path id of the last directory
int PathDAO::createPaths(QStringList& paths)
{
    QSqlQuery q;

    q.prepare("select id, directory ,parent_id from path where directory = :dir and ifnull(parent_id,-1) = :p_id");

    int key = insertPathRec(q, paths, 0, -1);

    // rebuild the tree
    rebuildPathTree(1, 1);

    emit pathsChanged();

    return key;
}

// recursively traverses the path, inserting directories in the table as needed.
// returns the path id of the last directory
int PathDAO::insertPathRec(QSqlQuery& q, const QStringList& path, int pos, int parentid)
{
    if (pos >= path.length())
        return parentid;

    QString p = path.at(pos);

    qDebug() << "Checking path:" << p;
    q.bindValue(":dir", p);
    q.bindValue(":p_id", parentid);

    if (q.exec())
    {
        int newparent;

        if (q.next())
        {
            newparent = q.value(0).toInt();
        }
        else
        {
            qDebug() << "Creating path";
            QSqlQuery iq;     // insert query
            iq.prepare("insert into path (directory,parent_id) values (:directory, :parent)");
            iq.bindValue(":directory", p);

            if (parentid == -1)
                iq.bindValue(":parent", QVariant(QVariant::LongLong));
            else
                iq.bindValue(":parent", parentid);
            iq.exec();
            newparent = iq.lastInsertId().toInt();
        }
        return insertPathRec(q, path, pos + 1, newparent);
    }
    return -1;
}

long long PathDAO::rebuildPathTree(long long parent_id, long long left)
{
    // the right value of this node is the left value + 1

    long long right = left + 1;

    // get all children of this node

    QSqlQuery q;

    q.prepare("select id from path where parent_id = :parent_id;");
    q.bindValue(":parent_id", parent_id);

    q.exec();

    while (q.next())
    {
        long long id = q.value(0).toLongLong();
        right = rebuildPathTree(id, right);
    }

    q.prepare("update path set lft=:left, rgt=:right where id=:parent_id;");

    q.bindValue(":left", left);
    q.bindValue(":right", right);
    q.bindValue(":parent_id", parent_id);

    q.exec();

    // return the right value of this node + 1

    return right + 1;
}
}
