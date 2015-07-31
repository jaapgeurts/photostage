#ifndef IMPORTWORKUNIT_H
#define IMPORTWORKUNIT_H

#include <QtSql>

#include "database/importinfo.h"

namespace PhotoStage
{
class ImportDAO
{
    public:

        static ImportDAO* instance();

        long long importPhoto(const QFileInfo& file, const ImportOptions& options);

        void beginImport();

        long long rebuildTree(long long parent_id, long long left);

    protected:

        explicit ImportDAO();

    signals:

    public slots:

    private:

        static ImportDAO* mInstance;

        // state variables for the import process
        QString   mLastpath;
        long long mLastkey;

        int createPaths(QStringList& paths);
        int insertPathRec(QSqlQuery& q, const QStringList& path, int pos, int parentid);
};
}
#endif // IMPORTWORKUNIT_H
