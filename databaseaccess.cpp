#include "databaseaccess.h"

#include <QtSql>
#include <QDebug>

DatabaseAccess::DatabaseAccess(QObject *parent) : QObject(parent)
{

    mDB = QSqlDatabase::addDatabase("QSQLITE");
    QString dbfile =  QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+QDir::separator()+"photostage.db";
    qDebug() << "Saving database to" << dbfile;
    mDB.setDatabaseName(dbfile);

    if (!mDB.open())
        qDebug() << mDB.lastError();

    QStringList tables = mDB.tables();
    if (tables.contains("photo", Qt::CaseInsensitive)
            && tables.contains("path", Qt::CaseInsensitive)
            && tables.contains("keyword", Qt::CaseInsensitive)
            && tables.contains("photo_keyword", Qt::CaseInsensitive))
        qDebug() << "Tables already exist.";
    else
    {
        initDb();
    }

}

const QSqlDatabase& DatabaseAccess::getDb()
{
    return mDB;
}

void DatabaseAccess::initDb()
{
    QSqlQuery q;
    if (!q.exec(QLatin1String("create table path (id integer primary key AUTOINCREMENT, directory varchar, parent_id integer)")))
        qDebug() << "Failed to create table" << q.lastError();
    if(!q.exec(QLatin1String("create table photo (id integer primary key AUTOINCREMENT, path_id integer, filename varchar, iso integer, shutter_speed float, float focal_length, datetime_taken text, hash varchar, rating integer, color integer, flag integer)")))
        qDebug() << "Failed to create table" << q.lastError();
    if(!q.exec(QLatin1String("create table keyword (id integer primary key AUTOINCREMENT, keyword varchar, parent_id integer)")))
        qDebug() << "Failed to create table" << q.lastError();
    if(!q.exec(QLatin1String("create table photo_keyword (photo_id integer, keyword_id integer)")))
        qDebug() << "Failed to create table" << q.lastError();

}



