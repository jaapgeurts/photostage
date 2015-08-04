#ifndef PHOTOSTAGE_COLLECTIONMODEL_H
#define PHOTOSTAGE_COLLECTIONMODEL_H

#include <QAbstractItemModel>

#include "database/collectiondao.h"

namespace PhotoStage
{
class CollectionModel : public QAbstractItemModel
{
    Q_OBJECT

    public:

        enum DataRole
        {
            CollectionRole = Qt::UserRole + 2
        };

        CollectionModel(QObject* parent = 0);
        ~CollectionModel();

        QModelIndex index(int row, int column, const QModelIndex& parent) const;
        QModelIndex parent(const QModelIndex& index) const;
        int rowCount(const QModelIndex& parent) const;
        int columnCount(const QModelIndex& parent) const;
        QVariant data(const QModelIndex& index, int role) const;

        Qt::DropActions supportedDropActions() const;

    private:

        CollectionItem* mRootItem;
};
}

#endif // PHOTOSTAGE_COLLECTIONMODEL_H
