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

    }
  }
}

QModelIndex AvailableDevicesModel::index(int row, int column, const QModelIndex& parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  if (!parent.isValid()) {
    // root item
    return createIndex(row,column,row);
  }
  return QModelIndex();
}


QVariant AvailableDevicesModel::data(const QModelIndex& index, int role) const
{
  // TODO: don't forget to filter
  //mSourceDevicesModel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);

  if (role == Qt::DisplayRole) {
    return mVolumes.at(index.internalId()).rootPath();
  }
  else
    return QVariant();
}


QModelIndex AvailableDevicesModel::parent(const QModelIndex& index) const
{
    return QModelIndex();
}

int AvailableDevicesModel::rowCount(const QModelIndex& parent) const
{
    return mVolumes.size();
}

int AvailableDevicesModel::columnCount(const QModelIndex&) const
{
  return 1;
}

QFileInfo AvailableDevicesModel::fileInfo(const QModelIndex& index) const
{
  return QFileInfo();
}
