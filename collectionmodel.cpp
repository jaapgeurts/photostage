#include "collectionmodel.h"

namespace PhotoStage
{
CollectionModel::CollectionModel(QObject* parent) :
    QAbstractItemModel(parent)
{
}

QModelIndex CollectionModel::index(int row, int column, const QModelIndex& parent) const
{
}

QModelIndex CollectionModel::parent(const QModelIndex& child) const
{
//    if (!index.isValid())
//        return QModelIndex();

//    CollectionItem* item = static_cast<CollectionItem*>(index.internalPointer());

//    if (item->parent == mRootItem)
//        return QModelIndex();

//    PathItem* newItem = item->parent;

//    return createIndex(0, 0, newItem);
}

int CollectionModel::rowCount(const QModelIndex& parent) const
{
}

int CollectionModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

QVariant CollectionModel::data(const QModelIndex& index, int role) const
{
}
}
