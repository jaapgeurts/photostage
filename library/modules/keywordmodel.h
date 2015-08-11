#ifndef PHOTOSTAGE_KEYWORDMODEL_H
#define PHOTOSTAGE_KEYWORDMODEL_H

#include <QAbstractItemModel>
#include <QtSql>

#include "database/keyworditem.h"

namespace PhotoStage
{
class KeywordModel : public QAbstractItemModel
{
    public:

        KeywordModel(QObject* parent = 0);
        ~KeywordModel();

    public:

        // QAbstractItemModel interface
        // minimum set to implement
        QModelIndex index(int row, int column, const QModelIndex& parent) const;
        QModelIndex parent(const QModelIndex& index) const;
        int rowCount(const QModelIndex& parent) const;
        int columnCount(const QModelIndex&) const;
        QVariant data(const QModelIndex& index, int role) const;

        // optional overrides
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;

        // implement when rowCount is expensive
        //    bool hasChildren(const QModelIndex &parent) const;

    private slots:

        void onKeywordsAdded();
        void onKeywordsDeleted();
        void onKeywordsChanged();

    private:

        KeywordItem* mRootItem;
};
}
#endif // PHOTOSTAGE_KEYWORDMODEL_H
