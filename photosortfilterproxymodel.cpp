#include "photosortfilterproxymodel.h"
#include "photo.h"

namespace PhotoStage
{
PhotoSortFilterProxyModel::PhotoSortFilterProxyModel(QObject* parent) :
    QSortFilterProxyModel(parent)
{
}

bool PhotoSortFilterProxyModel::filterAcceptsRow(int source_row,
    const QModelIndex& source_parent) const
{
    Photo p = sourceModel()->data(sourceModel()->index(source_row,
            0), Photo::DataRole).value<Photo>();

    if (mFilter.value("pick"))
        return p.flag() == Photo::FlagPick;
    else
        return true;
}

void PhotoSortFilterProxyModel::setFilter(const QString& filter)
{
    mFilter.insert("pick", filter.contains("pick"));
    invalidateFilter();
}
}
