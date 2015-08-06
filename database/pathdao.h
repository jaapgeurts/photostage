#ifndef PHOTOSTAGE_PATHDAO_H
#define PHOTOSTAGE_PATHDAO_H

#include <QString>
#include <QList>
#include <QSharedPointer>

#include "pathitem.h"

namespace PhotoStage
{
class DatabaseAccess;

class PathDAO : public QObject
{
    friend class DatabaseAccess;

    Q_OBJECT

    public:

        /**
         * @brief createPathItems - creates a full tree of the all pathitems.
         * @return The root item of the tree. must be deleted with deletePathItems();
         */PathItem* createPathItems();

        /**
         * @brief deletePathItems - Recursively deletes a tree starting with root
         * @param root
         */
        void deletePathItems(PathItem* root);

    private:

        PathDAO(QObject* parent = 0);

        void createPathItemsRec(PathItem* root);
};
}
#endif // PHOTOSTAGE_PATHDAO_H
