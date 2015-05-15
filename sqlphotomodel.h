#ifndef QPHOTOMODEL_H
#define QPHOTOMODEL_H

#include <QAbstractListModel>

class QPhotoModel : QAbstractListModel
{
public:
    QPhotoModel();
    ~QPhotoModel();

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    int rowCount(const QModelIndex &parent) const;

};

#endif // QPHOTOMODEL_H
