#ifndef PHOTOSTAGE_PHOTOWORKUNIT_H
#define PHOTOSTAGE_PHOTOWORKUNIT_H

#include <QStringList>
#include <QMap>
#include <QList>

#include "photo.h"
#include "importinfo.h"

namespace PhotoStage
{
class DatabaseAccess;

class PhotoDAO : public QObject
{
    friend class DatabaseAccess;

    Q_OBJECT

    public:

        void beginTransaction();
        void endTransaction();

        void setRating(const QList<Photo>& list, int rating);
        void setFlag(const QList<Photo>& list, Photo::Flag flag);
        void setColorLabel(const QList<Photo>& list, Photo::ColorLabel color);

        void insertKeywords(const QStringList& words);
        void assignKeywords(const QStringList& words, const QList<Photo>& list);
        void unAssignKeywordsExcept(const QStringList& words, const QList<Photo>& list);

        void beginImport();
        void importPhoto(const QFileInfo& file, const ImportOptions& options);

        QMap<QString, int> getPhotoKeywordsCount(const QList<Photo>& list) const;
        QStringList getPhotoKeywords(const Photo& photo) const;

        // Photo Items are created here.
        // ownership is transferred to the caller who should call delete on the objects
        QList<Photo> getPhotosByPath(long long path_id, bool includeSubDirs = true) const;

        QList<Photo> getPhotosByCollectionId(long long collection_id, bool includeSubDirs) const;

        QList<Photo> getPhotosById(QList<long long> idList) const;

        void deletePhotos(const QList<Photo>& list, bool deleteFile);

        void updateExifInfo(Photo& photo);
        void regenerateHash(Photo& photo);

        bool IsInLibrary(long long hash) const;

    signals:

        void keywordsAdded();
        void keywordsDeleted();
        void keywordsAssignmentChanged(const QList<Photo>& photos);

        void photosChanged(const QList<Photo>& photos);
        void photosAdded(long long pathid, long long photoid);
        void photosDeleted(const QList<Photo>& photos);

    private:

        QString   mLastPath;
        long long mLastPathId;

        PhotoDAO(QObject* parent = 0);

        long long rebuildKeywordTree(long long parent_id, long long left);
        void getLeftRightForPathId(long long path_id, long long& lft, long long& rgt) const;
        void getLeftRightForCollectionId(long long collection_id, long long& lft, long long& rgt) const;
        QString joinIds(const QList<long long>& idList) const;
};
}
#endif // PHOTOSTAGE_PHOTOWORKUNIT_H
