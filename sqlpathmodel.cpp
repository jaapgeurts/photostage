#include "sqlpathmodel.h"

SqlPathModel::SqlPathModel(QObject* parent) : QAbstractItemModel(parent)
{

    // Construct the file tree
    createPathItems();
}

SqlPathModel::~SqlPathModel()
{
    deletePathItems(mRootItem);
}

void SqlPathModel::createPathItems()
{
    // create table path (
    //    id integer primary key AUTOINCREMENT,
    //    directory varchar,
    //    parent integer)")))

    // first create the root item
    QSqlQuery query;

    QString queryText = QString("select id,directory,parent_id from path where parent_id is NULL");

    if (!query.exec(queryText))
        qDebug() << query.lastError();

    if (!query.first())
        qDebug() << "No records";

    mRootItem = new PathItem(query.value(0).toLongLong(),query.value(1).toString(),query.value(2).toLongLong());

    createPathtemsRec(mRootItem);
}

void SqlPathModel::createPathtemsRec(PathItem* root)
{
    QSqlQuery query;
    QString queryText = QString("select p.id, p.directory, p.parent_id, count(f.id) from path p left outer join photo f on p.id = f.path_id where  parent_id = :parent_id group by p.id, p.directory, p.parent_id");
    query.prepare(queryText);

    PathItem* item;

    query.bindValue(":parent_id",root->id);
    query.exec();

    //int total = 0;
    while (query.next())
    {
        // conside using std::shared_ptr
        item = new PathItem(query.value(0).toLongLong(),query.value(1).toString(), query.value(1).toLongLong());
        item->count = query.value(3).toInt();
        item->parent = root;
        root->children.append(item);
        createPathtemsRec(item);
        root->cumulative += item->count + item->cumulative;
    }
}

void SqlPathModel::deletePathItems(PathItem* root)
{
    PathItem* item;
    foreach(item, root->children)
    {
        deletePathItems(item);
    }
    root->children.clear();
    delete root;
}

QModelIndex SqlPathModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row,column,parent))
        return QModelIndex();

    PathItem* parentItem;

    if (!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<PathItem*>(parent.internalPointer());

    PathItem* newItem = parentItem->children.at(row);

    return createIndex(row,column,newItem);
}

QModelIndex SqlPathModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    PathItem* item = static_cast<PathItem*>(index.internalPointer());

    if (item->parent == mRootItem)
        return QModelIndex();

    PathItem* newItem = item->parent;

    return createIndex(0,0,newItem);
}

int SqlPathModel::rowCount(const QModelIndex &parent) const
{
    PathItem* item;

    if (!parent.isValid())
        item = mRootItem;
    else
        item = static_cast<PathItem*>(parent.internalPointer());

    return item->children.size();
}

int SqlPathModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 1;
}

QVariant SqlPathModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        qDebug() << "Requested item invalid";
        return QVariant();
    }

    PathItem* item = static_cast<PathItem*>(index.internalPointer());

    if (role == Qt::DisplayRole)
    {
        return QString("%1 (%2/%3)").arg(item->path, QString::number(item->count), QString::number(item->cumulative));
    }
    else if (role == SqlPathModel::Path)
    {
        return QVariant::fromValue<PathItem*>(item);
    }
    else
        return QVariant();
}

//bool SqlPathModel::hasChildren(const QModelIndex &parent) const
//{
//}

QVariant SqlPathModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const
{
    if (role == Qt::DisplayRole)
        return QString("Location");
    return QVariant();
}