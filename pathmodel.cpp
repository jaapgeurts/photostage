 #include "pathmodel.h"

namespace PhotoStage
{
PathModel::PathModel(QObject* parent) :
    QAbstractItemModel(parent)
{
    // Construct the file tree
    mRootItem = PathDAO::instance()->createPathItems();
}

PathModel::~PathModel()
{
    if (mRootItem != NULL)
        PathDAO::instance()->deletePathItems(mRootItem);
}

QModelIndex PathModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    PathItem* parentItem;

    if (!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<PathItem*>(parent.internalPointer());

    PathItem* newItem = parentItem->children.at(row);

    return createIndex(row, column, newItem);
}

// Short cut to return the model index for a particular pathitem id.
QModelIndex PathModel::index(long long pathid) const
{
    if (mRootItem == NULL)
        return QModelIndex(); // root has not been set yet.
    return findItemRec(mRootItem, 0, pathid);
}

QModelIndex PathModel::findItemRec(PathItem* item, int row, long long pathid) const
{
    if (item->id == pathid)
        return createIndex(row, 0, item);

    int j = 0;

    foreach(PathItem * i, item->children)
    {
        QModelIndex idx = findItemRec(i, j, pathid);

        if (idx.isValid())
            return idx;
        j++;
    }
    return QModelIndex();
}

QModelIndex PathModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    PathItem* item = static_cast<PathItem*>(index.internalPointer());

    if (item->parent == mRootItem)
        return QModelIndex();

    PathItem* newItem = item->parent;

    return createIndex(0, 0, newItem);
}

int PathModel::rowCount(const QModelIndex& parent) const
{
    PathItem* item;

    if (!parent.isValid())
        item = mRootItem;
    else
        item = static_cast<PathItem*>(parent.internalPointer());

    if (item != NULL)
        return item->children.size();
    else
        return 0;
}

int PathModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

QVariant PathModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    PathItem* item = static_cast<PathItem*>(index.internalPointer());

    switch (role)
    {
        case Qt::DisplayRole:
            return QString("%1 (%2/%3)").arg(item->path,
                       QString::number(item->count),
                       QString::number(item->cumulative));

        case PathModel::PathRole:
            return QVariant::fromValue<PathItem*>(item);

        case Qt::DecorationRole:
        default:
            return QVariant();
    }
}

//bool SqlPathModel::hasChildren(const QModelIndex &parent) const
//{
//}

QVariant PathModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const
{
    if (role == Qt::DisplayRole)
        return QString("Location");
    return QVariant();
}

void PathModel::reload()
{
    beginResetModel();

    if (mRootItem != NULL)
        PathDAO::instance()->deletePathItems(mRootItem);
    mRootItem = PathDAO::instance()->createPathItems();
    endResetModel();
}
}
