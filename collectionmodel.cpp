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
