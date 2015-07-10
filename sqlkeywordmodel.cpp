#include <algorithm>

#include "sqlkeywordmodel.h"

namespace PhotoStage
{
SqlKeywordModel::SqlKeywordModel(QObject* parent) :
    QAbstractItemModel(parent),
    mRootItem(NULL)
{
    // Construct the file tree
    createKeywordItems();
}

SqlKeywordModel::~SqlKeywordModel()
{
    if (mRootItem != NULL)
        deleteKeywordItems(mRootItem);
}

void SqlKeywordModel::createKeywordItems()
{
    // create table keyword
    // (id integer primary key AUTOINCREMENT,
    //   keyword varchar,
    //  parent_id integer

    // first create the root item
    QSqlQuery query;

    QString   queryText = QString(
        "select id,keyword,parent_id from keyword where parent_id is NULL");

    if (!query.exec(queryText))
        qDebug() << query.lastError();

    if (query.first())
    {
        mRootItem = new KeywordItem(query.value(0).toLongLong(),
                query.value(1).toString(),
                query.value(2).toLongLong());
        createKeywordItemsRec(mRootItem);
    }
}

bool KComp(const KeywordItem* const& a, const KeywordItem* const& b)
{
    int c = QString::compare(a->keyword, b->keyword, Qt::CaseInsensitive);

    return c < 0;
}

void SqlKeywordModel::createKeywordItemsRec(KeywordItem* root)
{
    QSqlQuery query;
    QString   queryText = QString(
        "select id,keyword,parent_id from keyword where parent_id = :parent_id");

    query.prepare(queryText);

    KeywordItem* item;

    query.bindValue(":parent_id", root->id);
    query.exec();

    while (query.next())
    {
        item = new KeywordItem(query.value(0).toLongLong(),
                query.value(1).toString(),
                query.value(1).toLongLong());
        item->parent = root;
        root->children.append(item);
        createKeywordItemsRec(item);
    }
    // sort the children
    std::sort(root->children.begin(), root->children.end(), KComp);
}

void SqlKeywordModel::deleteKeywordItems(KeywordItem* root)
{
    KeywordItem* item;

    foreach(item, root->children)
    {
        deleteKeywordItems(item);
    }
    root->children.clear();
    delete root;
}

QModelIndex SqlKeywordModel::index(int row,
    int column,
    const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    KeywordItem* parentItem;

    if (!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<KeywordItem*>(parent.internalPointer());

    KeywordItem* newItem = parentItem->children.at(row);

    return createIndex(row, column, newItem);
}

QModelIndex SqlKeywordModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    KeywordItem* item = static_cast<KeywordItem*>(index.internalPointer());

    if (item->parent == mRootItem)
        return QModelIndex();

    KeywordItem* newItem = item->parent;

    return createIndex(0, 0, newItem);
}

int SqlKeywordModel::rowCount(const QModelIndex& parent) const
{
    KeywordItem* item;

    if (!parent.isValid())
        item = mRootItem;
    else
        item = static_cast<KeywordItem*>(parent.internalPointer());

    if (item != NULL)
        return item->children.size();
    else
        return 0;
}

int SqlKeywordModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

QVariant SqlKeywordModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        qDebug() << "Requested item invalid";
        return QVariant();
    }

    KeywordItem* item = static_cast<KeywordItem*>(index.internalPointer());

    if (role == Qt::DisplayRole)
        return item->keyword;
    else
        return QVariant();
}

//bool SqlKeywordModel::hasChildren(const QModelIndex &parent) const
//{
//}

QVariant SqlKeywordModel::headerData(int /*section*/,
    Qt::Orientation /*orientation*/,
    int role) const
{
    if (role == Qt::DisplayRole)
        return QString("Keywords");
    return QVariant();
}
}
