#include <QApplication>
#include <QDebug>
#include <QTextStream>
#include <QThread>
#include <QtSql>

#include "databaseaccess.h"

namespace PhotoStage {
DatabaseAccess* DatabaseAccess::mInstance = nullptr;

PhotoDAO*          DatabaseAccess::mPhotoDAO           = nullptr;
PathDAO*           DatabaseAccess::mPathDAO            = nullptr;
CollectionDAO*     DatabaseAccess::mCollectionDAO      = nullptr;
KeywordDAO*        DatabaseAccess::mKeywordDAO         = nullptr;
DevelopSettingDao* DatabaseAccess::mDevelopSettingsDAO = nullptr;

DatabaseAccess* DatabaseAccess::instance(QObject* parent)
{
  if (mInstance == NULL)
  {
    // Create the instance
    mInstance = new DatabaseAccess(parent);

    // create all DAOs
    mPhotoDAO           = new PhotoDAO(mInstance);
    mPathDAO            = new PathDAO(mInstance);
    mCollectionDAO      = new CollectionDAO(mInstance);
    mKeywordDAO         = new KeywordDAO(mInstance);
    mDevelopSettingsDAO = new DevelopSettingDao(mInstance);

    // keyword dao connections
    connect(mKeywordDAO, &KeywordDAO::keywordsAdded, mInstance,
            &DatabaseAccess::keywordsAdded);
    connect(mKeywordDAO, &KeywordDAO::keywordsDeleted, mInstance,
            &DatabaseAccess::keywordsDeleted);
    connect(mKeywordDAO, &KeywordDAO::keywordsAssignmentChanged, mInstance,
            &DatabaseAccess::keywordsAssignmentChanged);

    // photo dao connections
    connect(mPhotoDAO, &PhotoDAO::photosAdded, mInstance,
            &DatabaseAccess::photosAdded);
    connect(mPhotoDAO, &PhotoDAO::photosAdded, mInstance,
            &DatabaseAccess::pathsChanged);
    connect(mPhotoDAO, &PhotoDAO::photosDeleted, mInstance,
            &DatabaseAccess::onPhotosDeleted);
    connect(mPhotoDAO, &PhotoDAO::photosChanged, mInstance,
            &DatabaseAccess::photosChanged);

    // PathDAO connections
    connect(mPathDAO, &PathDAO::pathsChanged, mInstance,
            &DatabaseAccess::pathsChanged);

    // Collection connections
    connect(mCollectionDAO, &CollectionDAO::collectionAdded, mInstance,
            &DatabaseAccess::collectionAdded);
    connect(mCollectionDAO, &CollectionDAO::collectionsChanged, mInstance,
            &DatabaseAccess::collectionsChanged);
    connect(mCollectionDAO, &CollectionDAO::photosRemoved, mInstance,
            &DatabaseAccess::collectionPhotosRemoved);
  }
  return mInstance;
}

DatabaseAccess::DatabaseAccess(QObject* parent) : QObject(parent)
{
  QSqlDatabase dB = QSqlDatabase::addDatabase("QSQLITE");
  QString      dbfile =
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
      QDir::separator() + "photostage.db";
  qDebug() << "Saving database to" << dbfile;
  dB.setDatabaseName(dbfile);

  if (!dB.open())
    qDebug() << dB.lastError();

  QStringList tables = dB.tables();

  //    if (tables.contains("photo", Qt::CaseInsensitive)
  //        && tables.contains("path", Qt::CaseInsensitive)
  //        && tables.contains("keyword", Qt::CaseInsensitive)
  //        && tables.contains("collection", Qt::CaseInsensitive))
  //        qDebug() << "Tables already exist.";
  //    else
  {
    initDb();
  }
}

DatabaseAccess::~DatabaseAccess()
{
}

void DatabaseAccess::beginTransaction()
{
  QSqlDatabase::database().transaction();
}

void DatabaseAccess::endTransaction()
{
  QSqlDatabase::database().commit();
}

PathDAO* DatabaseAccess::pathDao()
{
  return mPathDAO;
}

CollectionDAO* DatabaseAccess::collectionDao()
{
  return mCollectionDAO;
}

KeywordDAO* DatabaseAccess::keywordDao()
{
  return mKeywordDAO;
}

DevelopSettingDao* DatabaseAccess::developSettingDao()
{
  return mDevelopSettingsDAO;
}

void DatabaseAccess::onPhotosDeleted(const QList<Photo>& photos)
{
  emit photosDeleted(photos);
  emit pathsChanged();
  emit collectionsChanged();

  // check if any photos are in a collection then emit collection changed as
  // well
  //    foreach(Photo photo, photos)
  //    {
  //        Nullable<long long> id =
  //        mCollectionDAO->collectionIdForPhoto(photo);

  //        if (id != nullptr)
  //            emit collectionChanged(id);
  //    }
}

PhotoDAO* DatabaseAccess::photoDao()
{
  return mPhotoDAO;
}

void DatabaseAccess::initDb()
{
#if defined(Q_OS_MAC)
  QFile f(":/db/database_schema.sql");
#elif defined(Q_OS_UNIX)
  QFile f(":/db/database_schema.sql");
#endif
  f.open(QFile::ReadOnly);
  QTextStream ts(&f);

  QSqlQuery q;
  QString   query = readQuery(ts);

  while (!query.isNull() && !query.isEmpty())
  {
    // qDebug() << "Running query:" << query;

    if (!q.exec(query))
    {
      qDebug() << "Query failed\n" << q.lastError();
      qDebug() << q.lastQuery() << "Stopping.";
      break;
    }
    query = readQuery(ts);
  }

  f.close();
}

QString DatabaseAccess::readQuery(QTextStream& ts)
{
  QString query;
  QString line;

  line = ts.readLine();

  while (!line.isNull())
  {
    if (!line.isEmpty() && line.length() > 1 && line.at(0) != '-' &&
        line.at(1) != '-')
    {
      query += " " + line.trimmed();

      if (query.at(query.length() - 1) == ';')
        return query;
    }
    line = ts.readLine();
  }
  return query;
}

bool DatabaseAccess::onMainThread()
{
  return QApplication::instance()->thread()->currentThreadId() ==
         QThread::currentThreadId();
}
} // namespace PhotoStage
