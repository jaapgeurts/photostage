#ifndef IMPORTWORKUNIT_H
#define IMPORTWORKUNIT_H

#include <QtSql>

#include "workunits/importinfo.h"

namespace PhotoStage
{
class ImportWorkUnit
{
    public:

        static ImportWorkUnit* instance();
        long long importPhoto(const QFileInfo& file,
            const ImportOptions& options);

        void beginImport();

        long long rebuildTree(long long parent_id, long long left);

    protected:

        explicit ImportWorkUnit();

    signals:

    public slots:

    private:

        static ImportWorkUnit* mInstance;

        // state variables for the import process
        QString   mLastpath;
        long long mLastkey;

        int createPaths(QStringList& paths);
        int insertPathRec(QSqlQuery& q,
            const QStringList& path,
            int pos,
            int parentid);
};
}
#endif // IMPORTWORKUNIT_H
