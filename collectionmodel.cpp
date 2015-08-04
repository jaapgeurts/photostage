#include "collectionmodel.h"

namespace PhotoStage
{
CollectionModel::CollectionModel(QObject* parent) :
    QAbstractItemModel(parent)
{
    mRootItem = CollectionDAO::instance()->createCollectionItems();
}

CollectionModel::~CollectionModel()
{
    if (mRootItem != NULL)
        CollectionDAO::instance()->deleteCollectionItems(mRootItem);
}

QModelIndex CollectionModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    CollectionItem* parentItem;

    if (!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<CollectionItem*>(parent.internalPointer());

    CollectionItem* newItem = parentItem->children.at(row);

    return createIndex(row, column, newItem);
}

QModelIndex CollectionModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    CollectionItem* item = static_cast<CollectionItem*>(index.internalPointer());

    if (item->parent == mRootItem)
        return QModelIndex();

    CollectionItem* newItem = item->parent;

    return createIndex(0, 0, newItem);
}

int CollectionModel::rowCount(const QModelIndex& parent) const
{
    CollectionItem* item;

    if (!parent.isValid())
        item = mRootItem;
    else
        item = static_cast<CollectionItem*>(parent.internalPointer());

    if (item != NULL)
        return item->children.size();
    else
        return 0;
}

int CollectionModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

QVariant CollectionModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    CollectionItem* item = static_cast<CollectionItem*>(index.internalPointer());

    switch (role)
    {
        case Qt::DisplayRole:
            return QString("%1 (%2/%3)").arg(item->name,
                       QString::number(item->count),
                       QString::number(item->cumulative));

        case CollectionModel::CollectionRole:
            return QVariant::fromValue<CollectionItem*>(item);

        case Qt::DecorationRole:
        default:
            return QVariant();
    }
}

Qt::DropActions CollectionModel::supportedDropActions() const
{
    return Qt::CopyAction;
}
}
