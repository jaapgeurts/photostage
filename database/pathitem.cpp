#include "pathitem.h"
#include "PathItem.h"

namespace PhotoStage
{
PathItem::PathItem() :
    path(),
    id(-1),
    parentId(-1),
    parent(NULL),
    count(0),
    cumulative(0),
    children()
{
}

PathItem::PathItem(long long id, const QString& path, long long parent_id) :
    path(path),
    id(id),
    parentId(parent_id),
    parent(NULL),
    count(0),
    cumulative(0),
    children()
{
}
}
