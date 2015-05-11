#include "qphotomodel.h"

QPhotoModel::QPhotoModel()
{

}

QPhotoModel::~QPhotoModel()
{

}


Qt::ItemFlags QPhotoModel::flags(const QModelIndex &index) const
{
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QPhotoModel::data(const QModelIndex &index, int role) const
{

}

QVariant QPhotoModel::headerData(int section, Qt::Orientation orientation, int role) const
{

}

int QPhotoModel::rowCount(const QModelIndex &parent) const
{

}
