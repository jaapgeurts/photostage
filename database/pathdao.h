#ifndef PHOTOSTAGE_PATHDAO_H
#define PHOTOSTAGE_PATHDAO_H

#include <QString>
#include <QList>
#include <QSharedPointer>

#include "pathitem.h"

namespace PhotoStage
{
class PathDAO
{
    public:

        static PathDAO* instance();

        /**
         * @brief createPathItems - creates a full tree of the all pathitems.
         * @return The root item of the tree. must be deleted with deletePathItems();
         */
        PathItem* createPathItems();
        /**
         * @brief deletePathItems - Recursively deletes a tree starting with root
         * @param root
         */
        void deletePathItems(PathItem* root);

    private:

        static PathDAO* mInstance;

        PathDAO();

        void createPathItemsRec(PathItem* root);
};
}
#endif // PHOTOSTAGE_PATHDAO_H
