#ifndef AVAILABLEDEVICESMODEL_H
#define AVAILABLEDEVICESMODEL_H

#include <QAbstractItemModel>
#include <QFileSystemModel>
#include <QStorageInfo>

#include <QObject>
namespace PhotoStage
{
class AvailableLocationsModel : public QAbstractItemModel
{
public:
  AvailableLocationsModel(QObject* parent = nullptr);

  QModelIndex index(int row, int column, const QModelIndex& parent) const;
  QModelIndex parent(const QModelIndex& index) const;
  int rowCount(const QModelIndex& parent) const;
  int columnCount(const QModelIndex&) const;
  QVariant data(const QModelIndex& index, int role) const;


private:
  QList<QDir> mSourceLocations;
};
}
#endif // AVAILABLEDEVICESMODEL_H
