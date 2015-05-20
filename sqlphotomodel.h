#ifndef QPHOTOMODEL_H
#define QPHOTOMODEL_H

#include <QAbstractListModel>
#include <QtSql>
#include <QHash>

#include "sqlphotoinfo.h"


class SqlPhotoModel : public QAbstractListModel
{
public:
    SqlPhotoModel(QObject* parent = 0);
    ~SqlPhotoModel();

    int rowCount(const QModelIndex &parent) const;
    QVariant headerData(int, Qt::Orientation, int) const;
    QVariant data(const QModelIndex &index, int role) const;

    /*
    // these methods are for editing
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    // these methods are for updating(add/remove)
    bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);
    */

    void updateData(const QList<QModelIndex> &list);

private slots:
    void imageLoaded(const QModelIndex &index, const QImage &pixmap);

private:
    QSqlQuery* mMainQuery;
    QHash<QModelIndex,SqlPhotoInfo> *mPhotoInfoCache;
    QThreadPool *mThreadPool;

};

#endif // QPHOTOMODEL_H
