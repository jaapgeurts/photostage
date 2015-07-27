#include "photosortfilterproxymodel.h"
#include "photo.h"

namespace PhotoStage
{
PhotoSortFilterProxyModel::PhotoSortFilterProxyModel(QObject* parent) :
    QSortFilterProxyModel(parent),
    mFilterInfo()
{
}

bool PhotoSortFilterProxyModel::filterAcceptsRow(int source_row,
    const QModelIndex& source_parent) const
{
    Photo p = sourceModel()->data(sourceModel()
            ->index(source_row, 0), Photo::DataRole).value<Photo>();

    bool flag    = false;
    bool rating  = false;
    bool keyword = false;

    if (mFilterInfo.flagNone == mFilterInfo.flagPick == mFilterInfo.flagReject == false)
    {
        flag = true; // user didn't select any flag. Take this as any flag
    }
    else
    { // check individual flags
        if (mFilterInfo.flagPick == p.flag())
            flag = true;

        if (mFilterInfo.flagReject == p.flag())
            flag = true;

        if (mFilterInfo.flagNone == p.flag()) // the item has no flag
            flag = true;
    }

    if (mFilterInfo.rating == 0) // any rating
        rating = true;
    else if (mFilterInfo.rating == p.rating())
        rating = true;

    return flag && rating;

    if (mFilterInfo.keywordsNone)
    {
        keyword = p.keywords().size() == 0;
    }
    else
    {
        // check keywords from DB
        QListIterator<QString> iter(mFilterInfo.keywords);
        bool                   illegalkw = false;

        while (iter.hasNext())
        {
            if (!p.keywords().contains(iter.next(), Qt::CaseInsensitive))
            {
                illegalkw = true;
                break;
            }
        }
        keyword = !illegalkw;
    }
    return flag && rating && keyword;
}

void PhotoSortFilterProxyModel::setFilter(const PhotoFilterInfo& info)
{
    mFilterInfo = info;
    invalidateFilter();
}
}
