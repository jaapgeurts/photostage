#ifndef IMPORTWORKUNIT_H
#define IMPORTWORKUNIT_H

#include <QObject>
#include <QtSql>

#include "importinfo.h"

class ImportWorkUnit : public QObject
{
    Q_OBJECT
public:
    explicit ImportWorkUnit(QObject *parent = 0);

    void importPhotos(const ImportInfo &info);

signals:

public slots:

private:
    int createPaths(QStringList &paths);
    int insertPath(QSqlQuery &q, const QStringList &path, int pos, int parentid);
    int importPhoto(const QFileInfo &file, int pathkey);
};

#endif // IMPORTWORKUNIT_H
