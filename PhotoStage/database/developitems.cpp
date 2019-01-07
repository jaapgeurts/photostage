#include "developitems.h"

#include "constants.h"

namespace PhotoStage
{
DevelopHistoryItem::DevelopHistoryItem() :
    id(-1)
{
}

DevelopHistoryItem::DevelopHistoryItem(long long id, long long moduleType, const QString& text, bool isMutable) :
    id(id),
    friendlyText(text),
    moduleType(moduleType),
    isMutable(isMutable)
{
}

DevelopHistory::DevelopHistory()
{
}

DevelopHistory::DevelopHistory(const QList<QSharedPointer<DevelopHistoryItem> >& list)
    : list(list)
{
}

QSharedPointer<DevelopParameterItem> DevelopHistory::rawSettings() const
{
    for (int i = 0; i < list.size(); i++)
    {
        const QSharedPointer<DevelopHistoryItem> item = list.at(i);

        if (item->moduleType == DEVELOP_SETTINGS_RAW)
            return item->developItem;
    }

    return QSharedPointer<DevelopParameterItem>();
}

bool DevelopParameterItem::isNull() const
{
    return true;
}

DevelopParameterItem::~DevelopParameterItem()
{
}
}
