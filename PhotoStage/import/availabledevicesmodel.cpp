#include <QDebug>
#include "availabledevicesmodel.h"

AvailableDevicesModel::AvailableDevicesModel(QObject* parent) :
  QAbstractItemModel(parent),
  mVolumes(QStorageInfo::mountedVolumes())
{
  // check if volume is valid and ready.
  foreach (const QStorageInfo& info, mVolumes) {
    if (info.isValid() && info.isReady()) {
      qDebug() << "Adding device: " << info.rootPath() << ":" << info.displayName();

      // create a filesystem model for each mountpoint
      QFileSystemModel* model = new QFileSystemModel(this);
      model->setRootPath(info.rootPath());
      mFileModels.append(model);
    }
  }
}

QModelIndex AvailableDevicesModel::index(int row, int column, const QModelIndex& parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  if (!parent.isValid()) {
    // root item
    QStorageInfo volume = mVolumes.at(row);
    return createIndex(row,column,nullptr);
  }
  else {
    mFileModels.at(row)->index(row,column,parent);
  }
}


QVariant AvailableDevicesModel::data(const QModelIndex& index, int role) const
{
  // TODO: don't forget to filter
  //mSourceDevicesModel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole) {
    if (index.model() == this)
      return mVolumes.at(index.row()).rootPath();
    else
      return mFileModels.at(index.row())->data(index,role);
  }
  else
    return QVariant();
}


QModelIndex AvailableDevicesModel::parent(const QModelIndex& index) const
{
  if (!index.isValid())
    return QModelIndex();

  if (index.model() == this)
    return QModelIndex();
  else
    return mFileModels.at(index.row())->parent(index);
}

int AvailableDevicesModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return mVolumes.size();

  return mFileModels.at(parent.row())->rowCount(parent);
}

int AvailableDevicesModel::columnCount(const QModelIndex&) const
{
  return 1;
}

QFileInfo AvailableDevicesModel::fileInfo(const QModelIndex& index) const
{
  return QFileInfo();
}
