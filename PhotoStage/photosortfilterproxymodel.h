#ifndef PHOTOSTAGE_PHOTOSORTFILTERPROXYMODEL_H
#define PHOTOSTAGE_PHOTOSORTFILTERPROXYMODEL_H

#include <QHash>
#include <QItemSelection>
#include <QSortFilterProxyModel>

#include "photo.h"
#include "photofilterinfo.h"

namespace PhotoStage {
class PhotoSortFilterProxyModel : public QSortFilterProxyModel
{
public:
  PhotoSortFilterProxyModel(QObject* parent = 0);

  void setFilter(const PhotoFilterInfo& info);

  QList<Photo> toList() const;

  QList<Photo> toList(const QItemSelection& selection) const;

protected:
  bool filterAcceptsRow(int source_row, const QModelIndex&) const;

private:
  PhotoFilterInfo mFilterInfo;
};
} // namespace PhotoStage
#endif // PHOTOSTAGE_PHOTOSORTFILTERPROXYMODEL_H
