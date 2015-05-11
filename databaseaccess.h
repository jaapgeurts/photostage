#ifndef DATABASEACCESS_H
#define DATABASEACCESS_H

#include <QtSql>

class DatabaseAccess : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseAccess(QObject *parent = 0);


signals:

public slots:

private:
    QSqlDatabase mDB;
    void initDb();
};

#endif // DATABASEACCESS_H
