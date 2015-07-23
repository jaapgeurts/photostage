#ifndef PHOTOSTAGE_PHOTOSORTFILTERPROXYMODEL_H
#define PHOTOSTAGE_PHOTOSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QHash>

#include "photofilterinfo.h"

namespace PhotoStage
{
class PhotoSortFilterProxyModel : public QSortFilterProxyModel
{
    public:

        PhotoSortFilterProxyModel(QObject* parent = 0);

        void setFilter(const PhotoFilterInfo& info);

    protected:

        bool filterAcceptsRow(int source_row,
            const QModelIndex& source_parent) const;

    private:

        PhotoFilterInfo mFilterInfo;
};
}
#endif // PHOTOSTAGE_PHOTOSORTFILTERPROXYMODEL_H