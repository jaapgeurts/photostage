#ifndef PHOTOWORKUNIT_H
#define PHOTOWORKUNIT_H

#include <QStringList>
#include <QMap>
#include <QList>

#include "photo.h"

namespace PhotoStage
{
class PhotoDAO
{
    public:

        static PhotoDAO* instance();

        void beginTransaction();
        void endTransaction();

        void setRating(const QList<Photo>& list, int rating);
        void setFlag(const QList<Photo>& list, Photo::Flag flag);
        void setColorLabel(const QList<Photo>& list, Photo::ColorLabel color);

        void insertKeywords(const QStringList& words);
        void assignKeywords(const QStringList& words, const QList<Photo>& list);
        void removeKeywordsExcept(const QStringList& words, const QList<Photo>& list);

        QMap<QString, int> getPhotoKeywordsCount(const QList<Photo>& list) const;
        QStringList getPhotoKeywords(const Photo& photo) const;

        // Photo Items are created here.
        // ownership is transferred to the caller who should call delete on the objects
        QList<Photo> getPhotosByPath(long long path_id, bool includeSubDirs = true) const;

        QList<Photo> getPhotosById(QList<long long> idList) const;
        void filterList(QList<long long>& list, long long rootPathId, bool includeSubDirs) const;

        void deletePhotos(const QList<Photo>& list, bool deleteFile);

        void updateExifInfo(const Photo& photo) const;
        void regenerateHash(Photo& p);

        bool IsInLibrary(long long hash) const;

    protected:

        PhotoDAO();

    private:

        static PhotoDAO* mInstance;

        long long rebuildTree(long long parent_id, long long left);
        void getLeftRightForPathId(long long path_id, long long& lft, long long& rgt) const;
        QString joinIds(const QList<long long>& idList) const;
};
}
#endif // PHOTOWORKUNIT_H
