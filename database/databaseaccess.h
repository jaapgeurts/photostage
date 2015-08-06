#ifndef PHOTOSTAGE_DATABASEACCESS_H
#define PHOTOSTAGE_DATABASEACCESS_H

#include <QtSql>

#include "collectiondao.h"
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
        static PathDAO* pathDao();
        static CollectionDAO* collectionDao();

    signals:

        void keywordsAdded();
        void keywordsDeleted();
        void keywordsAssignmentChanged(const QList<Photo>& photos);

        void photosChanged(const QList<Photo>& photos);
        void photosAdded(long long pathid, long long photoid);
        void photosDeleted(const QList<Photo>& photos);

        void pathsChanged();

        void collectionAdded();
        void collectionsChanged();

    public slots:

        void onPhotosDeleted(const QList<Photo>& photos);

    private:

        static DatabaseAccess* mInstance;

        QSqlDatabase           mDB;

        static PhotoDAO*       mPhotoDAO;
        static PathDAO*        mPathDAO;
        static CollectionDAO*  mCollectionDAO;

        explicit DatabaseAccess(QObject* parent = 0);

        void initDb();
};
}
#endif // DATABASEACCESS_H
