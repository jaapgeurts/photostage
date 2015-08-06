#ifndef PHOTOSTAGE_COLLECTIONMODEL_H
#define PHOTOSTAGE_COLLECTIONMODEL_H

#include <QAbstractItemModel>

#include "database/databaseaccess.h"

namespace PhotoStage
{
class CollectionModel : public QAbstractItemModel
{
    Q_OBJECT

    public:

        enum DataRole
        {
            CollectionRole = Qt::UserRole + 500
        };

        // Constructors & Destructor
        CollectionModel(QObject* parent = 0);
        ~CollectionModel();

        // Data access methods
        QModelIndex index(int row, int column, const QModelIndex& parent) const;
        QModelIndex parent(const QModelIndex& index) const;
        int rowCount(const QModelIndex& parent) const;
        int columnCount(const QModelIndex& parent) const;
        QVariant data(const QModelIndex& index, int role) const;

        // Drag Drop related methods
        Qt::DropActions supportedDropActions() const;
        Qt::ItemFlags flags(const QModelIndex& index) const;
        QStringList mimeTypes() const;
        QMimeData* mimeData(const QModelIndexList& indexes) const;
        bool canDropMimeData(const QMimeData* mimeData,
            Qt::DropAction action, int row, int column, const QModelIndex& parent) const;
        bool dropMimeData(const QMimeData* mimeData,
            Qt::DropAction action, int row, int column, const QModelIndex& parent);

private slots:
        void onCollectionsChanged();

    private:

        CollectionItem* mRootItem;
};
}

#endif // PHOTOSTAGE_COLLECTIONMODEL_H
