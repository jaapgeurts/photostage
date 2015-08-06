#ifndef PHOTOSTAGE_DATABASEACCESS_H
#define PHOTOSTAGE_DATABASEACCESS_H

#include <QtSql>

#include "collectiondao.h"
#include "importdao.h"
#include "pathdao.h"
#include "photodao.h"

namespace PhotoStage
{
class DatabaseAccess : public QObject
{
    Q_OBJECT

    public:

        static DatabaseAccess* instance(QObject* parent = 0);

        ~DatabaseAccess();

        const QSqlDatabase& getDb() const;

        static PhotoDAO* photoDao();
        static ImportDAO* importDao();
        static PathDAO* pathDao();
        static CollectionDAO* collectionDao();

    signals:

        void a();

    public slots:

    private:

        static DatabaseAccess* mInstance;

        QSqlDatabase           mDB;

        static PhotoDAO*       mPhotoDAO;
        static ImportDAO*      mImportDAO;
        static PathDAO*        mPathDAO;
        static CollectionDAO*  mCollectionDAO;

        explicit DatabaseAccess(QObject* parent = 0);

        void initDb();
};
}
#endif // DATABASEACCESS_H
