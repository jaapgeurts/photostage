#ifndef AVAILABLEDEVICESMODEL_H
#define AVAILABLEDEVICESMODEL_H

#include <QAbstractItemModel>
#include <QFileSystemModel>
#include <QStorageInfo>

#include <QObject>

class AvailableDevicesModel : public QAbstractItemModel
{
public:
  AvailableDevicesModel(QObject* parent = nullptr);

  QModelIndex index(int row, int column, const QModelIndex& parent) const;
  QModelIndex parent(const QModelIndex& index) const;
  int rowCount(const QModelIndex& parent) const;
  int columnCount(const QModelIndex&) const;
  QVariant data(const QModelIndex& index, int role) const;

  QFileInfo fileInfo(const QModelIndex& index) const;

private:
  QList<QStorageInfo> mVolumes;
};

#endif // AVAILABLEDEVICESMODEL_H
