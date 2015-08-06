#ifndef PHOTOSTAGE_IMPORTWORKUNIT_H
#define PHOTOSTAGE_IMPORTWORKUNIT_H

#include <QtSql>

#include "database/importinfo.h"

namespace PhotoStage
{
class DatabaseAccess;

class ImportDAO : public QObject
{
    friend class DatabaseAccess;

    Q_OBJECT

    public:

        long long importPhoto(const QFileInfo& file, const ImportOptions& options);

        void beginImport();

        long long rebuildTree(long long parent_id, long long left);

    signals:

    public slots:

    private:

        explicit ImportDAO(QObject* parent = 0);

        // state variables for the import process
        QString   mLastpath;
        long long mLastkey;

        int createPaths(QStringList& paths);
        int insertPathRec(QSqlQuery& q, const QStringList& path, int pos, int parentid);
};
}
#endif // IMPORTWORKUNIT_H
