#ifndef PHOTOSTAGE_PHOTOSORTFILTERPROXYMODEL_H
#define PHOTOSTAGE_PHOTOSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QItemSelection>
#include <QHash>

#include "photo.h"
#include "photofilterinfo.h"

namespace PhotoStage
{
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
}
#endif // PHOTOSTAGE_PHOTOSORTFILTERPROXYMODEL_H
