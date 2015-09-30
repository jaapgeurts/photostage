#ifndef PHOTOSTAGE_DATABASEACCESS_H
#define PHOTOSTAGE_DATABASEACCESS_H

#include <QtSql>

#include "collectiondao.h"
#include "pathdao.h"
#include "photodao.h"
#include "keyworddao.h"
#include "developsettingdao.h"

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
        static KeywordDAO* keywordDao();
        static DevelopSettingDao* developSettingDao();

        void beginTransaction();
        void endTransaction();

    signals:

        void keywordsAdded();
        void keywordsDeleted();
        void keywordsAssignmentChanged(const QList<Photo>& photos);

        void photosChanged(const QList<Photo>& photos);
        void photosAdded(long long pathid, const QList<long long>& list);
        void photosDeleted(const QList<Photo>& photos);

        void pathsChanged();

        void collectionAdded(long long id);
        void collectionsChanged();
        void collectionPhotosRemoved(long long collectionid, const QList<Photo>& list);

    public slots:

        void onPhotosDeleted(const QList<Photo>& photos);

    private:

        static DatabaseAccess*    mInstance;

        QSqlDatabase              mDB;

        static PhotoDAO*          mPhotoDAO;
        static PathDAO*           mPathDAO;
        static CollectionDAO*     mCollectionDAO;
        static KeywordDAO*        mKeywordDAO;
        static DevelopSettingDao* mDevelopSettingsDAO;

        explicit DatabaseAccess(QObject* parent = 0);

        void initDb();
        QString readQuery(QTextStream& ts);
};
}
#endif // DATABASEACCESS_H
