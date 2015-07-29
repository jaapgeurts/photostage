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
    bool color   = false;

    if (!mFilterInfo.flagNone && !mFilterInfo.flagPick && !mFilterInfo.flagReject)
    {
        flag = true; // user didn't select any flag. Take this as any flag
    }
    else
    { // check individual flags
        if (mFilterInfo.flagPick)
            flag = p.flag() == Photo::FlagPick;

        if (mFilterInfo.flagReject)
            flag = flag || p.flag() == Photo::FlagReject;

        if (mFilterInfo.flagNone)
            flag = flag || p.flag() == Photo::FlagNone; // the item has no flag
    }

    if (!mFilterInfo.colorBlue && !mFilterInfo.colorGreen && !mFilterInfo.colorRed && !mFilterInfo.colorYellow)
    {
        color = true; // if no color selected, take this as all colors
    }
    else
    {
        //check individual colors
        if (mFilterInfo.colorBlue)
            color = p.colorLabel() == Photo::LabelBlue;

        if (mFilterInfo.colorGreen)
            color = color || p.colorLabel() == Photo::LabelGreen;

        if (mFilterInfo.colorRed)
            color = color || p.colorLabel() == Photo::LabelRed;

        if (mFilterInfo.colorYellow)
            color =  color || p.colorLabel() == Photo::LabelYellow;

        if (mFilterInfo.colorPurple)
            color = color || p.colorLabel() == Photo::LabelPurple;

        if (mFilterInfo.colorOrange)
            color = color || p.colorLabel() == Photo::LabelOrange;
    }

    if (mFilterInfo.rating == 0) // any rating
        rating = true;
    else if (mFilterInfo.rating == p.rating())
        rating = true;

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
    return flag && color && rating && keyword;
}

void PhotoSortFilterProxyModel::setFilter(const PhotoFilterInfo& info)
{
    mFilterInfo = info;
    invalidateFilter();
}

QList<Photo> PhotoSortFilterProxyModel::toList() const
{
    QList<Photo> list;
    int          c = rowCount();

    for (int i = 0; i < c; i++)
    {
        Photo p = data(index(i,0), Photo::DataRole).value<Photo>();
        list.append(p);
    }
    return list;
}

QList<Photo> PhotoSortFilterProxyModel::toList(const QItemSelection& selection) const
{
    QList<Photo> list;

    foreach(QModelIndex index, selection.indexes())
    {
        Photo p = data(index, Photo::DataRole).value<Photo>();

        list.append(p);
    }
    return list;
}
}
