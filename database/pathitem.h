#ifndef PHOTOSTAGE_PATHITEM_H
#define PHOTOSTAGE_PATHITEM_H

#include <QString>
#include <QList>
#include <QMetaType>

namespace PhotoStage
{
struct PathItem
{
    public:

        PathItem();
        PathItem(long long id, const QString& path, long long parent_id);

        QString          path;
        long long        id;
        long long        parentId;
        PathItem*        parent;
        int              count;
        int              cumulative;
        QList<PathItem*> children;
};
}
Q_DECLARE_METATYPE(PhotoStage::PathItem)
Q_DECLARE_METATYPE(PhotoStage::PathItem*)

#endif // PHOTOSTAGE_PATHITEM_H
