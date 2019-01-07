#include "keyworditem.h"

namespace PhotoStage
{
KeywordItem::KeywordItem(long long id, const QString& keyword, long long parent_id) :
    keyword(keyword),
    id(id),
    parent_id(parent_id),
    parent(NULL)
{
}
}
