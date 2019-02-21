#ifndef PHOTOSTAGE_PHOTOWORKUNIT_H
#define PHOTOSTAGE_PHOTOWORKUNIT_H

#include <QList>

#include "photo.h"

namespace PhotoStage
{
class DatabaseAccess;

class PhotoDAO : public QObject
{
    friend class DatabaseAccess;

    Q_OBJECT

    public:

        void setRating(const QList<Photo>& list, int rating);
        void setFlag(const QList<Photo>& list, Photo::Flag flag);
        void setColorLabel(const QList<Photo>& list, Photo::ColorLabel color);
        void assignGeoCoordinate(const QGeoCoordinate& coord, const QList<Photo>& list);

        void importPhoto(long long collectionid, int pathId, const QString& fileName, long long hash, const ExifInfo& exifInfo);

        // Photo Items are created here.
        // ownership is transferred to the caller who should call delete on the objects
        QList<Photo> getPhotosByPath(long long path_id, bool includeSubDirs = true) const;

        QList<Photo> getPhotosByCollectionId(long long collection_id, bool includeSubDirs) const;

        QList<Photo> getPhotosById(const QList<long long> idList) const;

        void deletePhotos(const QList<Photo>& list, bool deleteFile);

        void updateExifInfo(Photo& photo);
        void regenerateHash(Photo& photo);

        bool IsInLibrary(long long hash) const;

    signals:

        void photosChanged(const QList<Photo>& photos);
        void photosAdded(long long pathid, const QList<long long>& photos);
        void photosDeleted(const QList<Photo>& photos);

    private:


        explicit PhotoDAO(QObject* parent = 0);

        void getLeftRightForPathId(long long path_id, long long& lft, long long& rgt) const;
        void getLeftRightForCollectionId(long long collection_id, long long& lft, long long& rgt) const;
};
}
#endif // PHOTOSTAGE_PHOTOWORKUNIT_H
