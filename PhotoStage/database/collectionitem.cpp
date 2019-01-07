#include "collectionitem.h"

namespace PhotoStage
{
CollectionItem::CollectionItem() :
    name(),
    id(-1),
    parentId(-1),
    parent(NULL),
    count(0),
    cumulative(0),
    children()
{
}

CollectionItem::CollectionItem(long long id, const QString& name, long long parent_id) :
    name(name),
    id(id),
    parentId(parent_id),
    parent(NULL),
    count(0),
    cumulative(0),
    children()
{
}
}
