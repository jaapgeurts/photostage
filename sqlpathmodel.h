#ifndef SQLPATHMODEL_H
#define SQLPATHMODEL_H

#include <QtSql>
#include <QAbstractItemModel>


struct PathItem
{
    PathItem(long long id, QString path, long long parent_id)
    {
        this->id = id; this->path = path; this->parent_id = parent_id; parent = NULL;
        count = 0;
        cumulative = 0;
    }

    QString path;
    long long id;
    long long parent_id;
    struct PathItem *parent;
    int count;
    int cumulative;
    QList<struct PathItem*> children;
};

class SqlPathModel : public QAbstractItemModel
{
public:
    SqlPathModel(QObject* parent=0);
    ~SqlPathModel();

public:
    // QAbstractItemModel interface
    // minimum set to implement
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    // optional overrides
    QVariant headerData(int, Qt::Orientation, int role) const;


    // implement when rowCount is expensive
    //    bool hasChildren(const QModelIndex &parent) const;

private:

    void deletePathItems(PathItem *root);

    PathItem* mRootItem;

    void createPathtemsRec(PathItem *root);
    void createPathItems();
};

#endif // SQLPATHMODEL_H
