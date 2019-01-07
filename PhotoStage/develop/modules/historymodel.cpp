#include "historymodel.h"

#include "database/databaseaccess.h"

namespace PhotoStage
{
HistoryModel::HistoryModel(QWidget* parent) :
    QAbstractListModel(parent)
{
    DatabaseAccess* dbAccess = DatabaseAccess::instance();
}

void HistoryModel::setPhoto(const Photo& photo)
{
    mPhoto          = photo;
    mDevelopHistory = DatabaseAccess::developSettingDao()->getDevelopHistory(mPhoto);

    emit dataChanged(QModelIndex(), QModelIndex());
}

int HistoryModel::rowCount(const QModelIndex& parent) const
{
    if (mDevelopHistory.isNull())
        return 0;

    return mDevelopHistory->list.size();
}

QVariant HistoryModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role)
    {
        case Qt::DisplayRole:
            return mDevelopHistory->list.at(index.row())->friendlyText;

        default:
            return QVariant();
    }
}
}
