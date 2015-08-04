#ifndef SQLPATHMODEL_H
#define SQLPATHMODEL_H

#include <QAbstractItemModel>
#include <QMetaType>

#include "database/pathdao.h"

namespace PhotoStage
{
class PathModel : public QAbstractItemModel
{
    Q_OBJECT

    public:

        enum DataRole
        {
            PathRole = Qt::UserRole + 1
        };
        PathModel(QObject* parent = 0);
        ~PathModel();

    public:

        // QAbstractItemModel interface
        // minimum set to implement
        QModelIndex index(int row, int column, const QModelIndex& parent) const;
        QModelIndex index(long long pathid) const;
        QModelIndex parent(const QModelIndex& index) const;
        int rowCount(const QModelIndex& parent) const;
        int columnCount(const QModelIndex& parent) const;
        QVariant data(const QModelIndex& index, int role) const;

        // optional overrides
        QVariant headerData(int, Qt::Orientation, int role) const;

        // implement when rowCount is expensive
        //    bool hasChildren(const QModelIndex &parent) const;

    public slots:

        void reload();

    private:

        PathItem* mRootItem;

        QModelIndex findItemRec(PathItem* item, int row, long long pathid) const;
};
}

#endif // SQLPATHMODEL_H
