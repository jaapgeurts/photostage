#ifndef IMPORTWORKUNIT_H
#define IMPORTWORKUNIT_H

#include <QtSql>

#include "workunits/importinfo.h"

class ImportWorkUnit
{

public:
    static ImportWorkUnit* instance();
    long long importPhoto(const QFileInfo& file, const ImportOptions& options);

    void beginImport();
protected:
     explicit ImportWorkUnit();

signals:

public slots:

private:
    static ImportWorkUnit* mInstance;

    // state variables for the import process
    QString mLastpath;
    long long mLastkey;

    int createPaths(QStringList &paths);
    int insertPathRec(QSqlQuery &q, const QStringList &path, int pos, int parentid);
};

#endif // IMPORTWORKUNIT_H
