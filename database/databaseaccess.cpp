#include "databaseaccess.h"

#include <QtSql>
#include <QDebug>

namespace PhotoStage
{
DatabaseAccess* DatabaseAccess::mInstance      = NULL;
PhotoDAO*       DatabaseAccess::mPhotoDAO      = NULL;
ImportDAO*      DatabaseAccess::mImportDAO     = NULL;
PathDAO*        DatabaseAccess::mPathDAO       = NULL;
CollectionDAO*  DatabaseAccess::mCollectionDAO = NULL;

DatabaseAccess* DatabaseAccess::instance(QObject* parent)
{
    if (mInstance == NULL)
    {
        // Create the instance
        mInstance = new DatabaseAccess(parent);

        // create all DAOs
        mPhotoDAO      = new PhotoDAO(mInstance);
        mPathDAO       = new PathDAO(mInstance);
        mCollectionDAO = new CollectionDAO(mInstance);
        mImportDAO     = new ImportDAO(mInstance);

        // connect all signals and slots
    }
    return mInstance;
}

DatabaseAccess::DatabaseAccess(QObject* parent) :
    QObject(parent)
{
    mDB = QSqlDatabase::addDatabase("QSQLITE");
    QString dbfile =  QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
        + QDir::separator() + "photostage.db";
    qDebug() << "Saving database to" << dbfile;
    mDB.setDatabaseName(dbfile);

    if (!mDB.open())
        qDebug() << mDB.lastError();

    QStringList tables = mDB.tables();

    if (tables.contains("photo", Qt::CaseInsensitive)
        && tables.contains("path", Qt::CaseInsensitive)
        && tables.contains("keyword", Qt::CaseInsensitive)
        && tables.contains("photo_keyword", Qt::CaseInsensitive)
        && tables.contains("collection", Qt::CaseInsensitive)
        && tables.contains("import_history", Qt::CaseInsensitive))
        qDebug() << "Tables already exist.";
    else
    {
        initDb();
    }
}

DatabaseAccess::~DatabaseAccess()
{
}

const QSqlDatabase& DatabaseAccess::getDb() const
{
    return mDB;
}

ImportDAO* DatabaseAccess::importDao()
{
    return mImportDAO;
}

PathDAO* DatabaseAccess::pathDao()
{
    return mPathDAO;
}

CollectionDAO* DatabaseAccess::collectionDao()
{
    return mCollectionDAO;
}

PhotoDAO* DatabaseAccess::photoDao()
{
    return mPhotoDAO;
}

void DatabaseAccess::initDb()
{
    QStringList list;

    list <<
        "CREATE TABLE if not exists path (id integer PRIMARY KEY AUTOINCREMENT, directory varchar, parent_id integer REFERENCES path (id), lft INTEGER NOT NULL DEFAULT (0), rgt INTEGER NOT NULL DEFAULT (0));";
    list <<
        "CREATE TABLE if not exists photo (id integer PRIMARY KEY AUTOINCREMENT, path_id integer REFERENCES path (id), filename TEXT NOT NULL, iso integer, exposure_time float, focal_length float, datetime_original TEXT, photo_hash TEXT, rating integer, color integer, flag integer, rotation INTEGER, longitude float, lattitude float, datetime_digitized TEXT, copyright TEXT, artist TEXT, aperture float, flash BOOLEAN, lens_name TEXT, make TEXT, model TEXT, width integer, height integer);";
    list <<
        "CREATE TABLE if not exists keyword (id integer PRIMARY KEY AUTOINCREMENT, keyword varchar NOT NULL, parent_id integer REFERENCES keyword (id), lft INTEGER NOT NULL DEFAULT (0), rgt INTEGER NOT NULL DEFAULT (0));";
    list <<
        "create unique index if not exists idx_keyword on keyword (keyword, parent_id)";
    list <<
        "CREATE TABLE if not exists photo_keyword (photo_id integer REFERENCES photo (id), keyword_id integer REFERENCES keyword (id));";
    list <<
        "create unique index if not exists idx_photo_keyword on photo_keyword (photo_id, keyword_id)";
    list <<
        "create table if not exists collection (id integer primary key AUTOINCREMENT, name varchar, parent_id integer, lft integer, rgt integer)";

    list <<
        "create table if not exists work (id integer primary key AUTOINCREMENT, parent_id integer)";
    list <<
        "create table if not exists work_photo (work_id integer, photo_id integer)";

    list <<
        "create table if not exists importhistory (id integer primary key AUTOINCREMENT, date_time varchar)";
    list <<
        "create table if not exists importhistory_photo (importhistory_id integer, photo_id integer)";

    list <<
        "create table if not exists collection_photo ( collection_id integer REFERENCES collection(id), photo_id integer REFERENCES photo (id));";

    QSqlQuery q;
    foreach(QString query, list)
    {
        if (!q.exec(query))
        {
            qDebug() << "Query failed\n" << q.lastError() << "\n" <<
                q.lastQuery() << "Stopping.";
            break;
        }
    }

    // create a
}
}
