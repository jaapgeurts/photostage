#ifndef PHOTOSTAGE_COLLECTIONMODEL_H
#define PHOTOSTAGE_COLLECTIONMODEL_H

#include <QAbstractItemModel>

namespace PhotoStage
{
class CollectionModel : public QAbstractItemModel
{
    Q_OBJECT

    public:

        CollectionModel(QObject* parent = 0);

        // QAbstractItemModel interface

    public:

        QModelIndex index(int row, int column, const QModelIndex& parent) const;
        QModelIndex parent(const QModelIndex& child) const;
        int rowCount(const QModelIndex& parent) const;
        int columnCount(const QModelIndex& parent) const;
        QVariant data(const QModelIndex& index, int role) const;
};
}

#endif // PHOTOSTAGE_COLLECTIONMODEL_H
