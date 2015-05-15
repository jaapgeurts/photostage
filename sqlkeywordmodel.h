#ifndef SQLKEYWORDMODEL_H
#define SQLKEYWORDMODEL_H

#include <QAbstractItemModel>
#include <QtSql>


struct KeywordItem
{
    KeywordItem(long long id, QString keyword, long long parent_id)
    {
        this->id = id; this->keyword = keyword; this->parent_id = parent_id; parent = NULL;
    }

    QString keyword;
    long long id;
    long long parent_id;
    struct KeywordItem *parent;
    QList<struct KeywordItem*> children;

};

class SqlKeywordModel  : public QAbstractItemModel
{
public:
    SqlKeywordModel(QObject* parent = 0);
    ~SqlKeywordModel();

public:
    // QAbstractItemModel interface
    // minimum set to implement
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    // optional overrides
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;


    // implement when rowCount is expensive
    //    bool hasChildren(const QModelIndex &parent) const;

private:

    void deleteKeywordItems(KeywordItem *root);

    KeywordItem* mRootItem;

    void createKeywordItemsRec(KeywordItem *root);
    void createKeywordItems();
};

#endif // SQLKEYWORDMODEL_H
