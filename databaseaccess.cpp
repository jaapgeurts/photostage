#include "databaseaccess.h"

#include <QtSql>
#include <QDebug>

DatabaseAccess::DatabaseAccess(QObject *parent) : QObject(parent)
{

    mDB = QSqlDatabase::addDatabase("QSQLITE");
    mDB.setDatabaseName("photostage.db");

    if (!mDB.open())
        qDebug() << mDB.lastError().text();

    QStringList tables = mDB.tables();
    if (tables.contains("photos", Qt::CaseInsensitive)
            && tables.contains("paths", Qt::CaseInsensitive))
        qDebug() << "Tables already exist.";
     else
    {
        initDb();
    }

}

void DatabaseAccess::initDb()
{
    QSqlQuery q;
    q.exec(QLatin1String("create table photo (id integer primary key, path integer, filename varchar, iso integer, shutter_speed float, float focal_length, datetime_taken text, hash varchar, rating integer, color integer, flag integer"));
    q.exec(QLatin1String("create table path (id integer primary key, directory varchar, parent integer"));
    q.exec(QLatin1String("create table keyword (id integer primary key, keyword varchar, parent integer"));
    q.exec(QLatin1String("create table photo_keyword (photo_id integer, keyword_id integer)"));

}



