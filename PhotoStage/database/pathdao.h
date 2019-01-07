#ifndef PHOTOSTAGE_PATHDAO_H
#define PHOTOSTAGE_PATHDAO_H

#include <QtSql>
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
         */PathItem* getPathItems();

        /**
         * @brief deletePathItems - Recursively deletes a tree starting with root
         * @param root
         */
        void deletePathItems(PathItem* root);
        int createPaths(QStringList& paths);

    signals:

        void pathsChanged();

    private:

        explicit PathDAO(QObject* parent = 0);

        long long rebuildPathTree(long long parent_id, long long left);
        int insertPathRec(QSqlQuery& q, const QStringList& path, int pos, int parentid);
        void getPathItemsRec(PathItem* root);
};
}
#endif // PHOTOSTAGE_PATHDAO_H
