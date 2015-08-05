#include <QMimeData>

#include <QDebug>

#include "constants.h"
#include "dragdropinfo.h"
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
    return Qt::CopyAction | Qt::MoveAction;
}

Qt::ItemFlags CollectionModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDropEnabled | flags;
    else // only allow drops on valid items
        return flags;
}

QStringList CollectionModel::mimeTypes() const
{
    QStringList list;

    list << MIMETYPE_TILEVIEW_SELECTION;
    return list;
}

QMimeData* CollectionModel::mimeData(const QModelIndexList& indexes) const
{
    return NULL;
}

bool CollectionModel::canDropMimeData(const QMimeData* mimeData,
    Qt::DropAction action,
    int row,
    int column,
    const QModelIndex& parent) const
{
    if (action == Qt::IgnoreAction)
        return true;

    if (column > 0)
        return false;

    if (!parent.isValid())
        return false;

    if (!mimeData->hasFormat(MIMETYPE_TILEVIEW_SELECTION))
        return false;

    return true;
}

bool CollectionModel::dropMimeData(const QMimeData* mimeData,
    Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (column > 0)
        return false;

    if (!parent.isValid())
        return false;

    if (!mimeData->hasFormat(MIMETYPE_TILEVIEW_SELECTION))
        return false;

    DragDropInfo info(mimeData->data(MIMETYPE_TILEVIEW_SELECTION));

    if (info.sourceModel() == DragDropInfo::PhotoModel)
    {
        CollectionItem* item = data(parent, CollectionModel::CollectionRole).value<CollectionItem*>();
        qDebug() << "Dropped on" << item->name;
        qDebug() << "Items" << info.idList();
        return true;
    }
    else
    {
        return false;
    }
}
}
