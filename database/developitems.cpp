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

DevelopHistory::DevelopHistory(const QList<DevelopHistoryItem>& list)
    : mList(list)
{
}

DevelopParameterItem DevelopHistory::rawSettings() const
{
    for (int i = 0; i < mList.size(); i++)
    {
        const DevelopHistoryItem& item = mList.at(i);

        if (item.moduleType == DEVELOP_SETTINGS_RAW)
            return item.developItem;
    }

    return DevelopParameterItem();
}

bool DevelopParameterItem::isNull() const
{
    return true;
}
}
