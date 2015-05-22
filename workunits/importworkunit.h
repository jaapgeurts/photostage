#ifndef IMPORTWORKUNIT_H
#define IMPORTWORKUNIT_H

#include <QtSql>

#include "workunits/importinfo.h"

class ImportWorkUnit
{

public:
    static ImportWorkUnit* instance();
    bool importPhoto(const QFileInfo& info, const ImportOptions& options);

protected:
     explicit ImportWorkUnit();

signals:

public slots:

private:
    static ImportWorkUnit* mInstance;

    int createPaths(QStringList &paths);
    int insertPath(QSqlQuery &q, const QStringList &path, int pos, int parentid);
};

#endif // IMPORTWORKUNIT_H
