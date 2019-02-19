#include <QDebug>
#include <QStandardPaths>
#include "availablelocationsmodel.h"
#include "utils.h"
#include "support/qudev.h"

namespace PhotoStage
{
AvailableLocationsModel::AvailableLocationsModel(QObject* parent) :
  QAbstractItemModel(parent)
{
  // TODO: don't use hard coded locations anymore
  // add the user home location
  mSourceLocations << QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
  // and pictures
  mSourceLocations << QDir(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
  // and some standard locations
  mSourceLocations << QDir("/mnt");
  mSourceLocations << QDir("/run/mount");
  // always add root so users can access anything
  mSourceLocations << QDir("/");

/*
 *   QUDev udev;
  QList<QUDevDevice> list = udev.getDevices("block");

  // get all volumnes and check if it is valid and ready.
  QList<QStorageInfo> devices = QStorageInfo::mountedVolumes();
  foreach (const QStorageInfo& storage, devices) {
    if (storage.isValid() && storage.isReady()) {
#if defined(Q_OS_UNIX)
      if (is_block_device(storage.rootPath().toStdString())) {
#endif
        qDebug() << "Adding device: " << storage.rootPath();
        mSourceLocations << QDir(storage.rootPath());
#if defined(Q_OS_UNIX)
      }
#endif
    }
  }
  */
}

QModelIndex AvailableLocationsModel::index(int row, int column, const QModelIndex& parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  if (!parent.isValid()) {
    // root item
    return createIndex(row,column,row);
  }
  return QModelIndex();
}


QVariant AvailableLocationsModel::data(const QModelIndex& index, int role) const
{
  // TODO: don't forget to filter
  //mSourceDevicesModel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);

  if (role == Qt::DisplayRole) {
    return mSourceLocations.at(index.internalId()).absolutePath();
  }
  else
    return QVariant();
}


QModelIndex AvailableLocationsModel::parent(const QModelIndex& index) const
{
    return QModelIndex();
}

int AvailableLocationsModel::rowCount(const QModelIndex& parent) const
{
    return mSourceLocations.size();
}

int AvailableLocationsModel::columnCount(const QModelIndex&) const
{
  return 1;
}

}
