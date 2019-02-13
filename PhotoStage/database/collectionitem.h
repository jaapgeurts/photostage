#ifndef PHOTOSTAGE_COLLECTIONITEM_H
#define PHOTOSTAGE_COLLECTIONITEM_H

#include <QString>
#include <QList>
#include <QMetaType>

#include "collectiondao.h"

namespace PhotoStage
{
struct CollectionItem
{
    public:

        CollectionItem();
        CollectionItem(long long id, const QString& name, long long parent_id);

        QString                         name;
        long long                       id;
        long long                       parentId;
        CollectionItem*                 parent;
        int                             count;
        int                             cumulative;
        QList<CollectionItem*>          children;
        CollectionDAO::CollectionSource source;
};
}
Q_DECLARE_METATYPE(PhotoStage::CollectionItem)
Q_DECLARE_METATYPE(PhotoStage::CollectionItem*)

#endif // PHOTOSTAGE_COLLECTIONITEM_H