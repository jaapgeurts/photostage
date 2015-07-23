#include "photosortfilterproxymodel.h"
#include "photo.h"

namespace PhotoStage
{
PhotoSortFilterProxyModel::PhotoSortFilterProxyModel(QObject* parent) :
    QSortFilterProxyModel(parent)
{
    mFilterInfo.flag   = Photo::FlagNone;
    mFilterInfo.rating = 0;
}

bool PhotoSortFilterProxyModel::filterAcceptsRow(int source_row,
    const QModelIndex& source_parent) const
{
    Photo p = sourceModel()->data(sourceModel()
            ->index(source_row, 0), Photo::DataRole).value<Photo>();

    if (p.flag() != mFilterInfo.flag)
        return false;

    if (p.rating() != mFilterInfo.rating)
        return false;

    // check keywords from DB
    QListIterator<QString> iter(mFilterInfo.keywords);

    while (iter.hasNext())
    {
        if (!p.keywords().contains(iter.next(),Qt::CaseInsensitive))
            return false;
    }

    return true;
}

void PhotoSortFilterProxyModel::setFilter(const PhotoFilterInfo& info)
{
    mFilterInfo = info;
    invalidateFilter();
}
}
