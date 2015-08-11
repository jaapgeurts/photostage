#include "keyworditem.h"

namespace PhotoStage
{
KeywordItem::KeywordItem(long long id, const QString& keyword, long long parent_id) :
    id(id),
    keyword(keyword),
    parent_id(parent_id),
    parent(NULL)
{
}
}
