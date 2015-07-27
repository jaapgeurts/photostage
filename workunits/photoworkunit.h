#ifndef PHOTOWORKUNIT_H
#define PHOTOWORKUNIT_H

#include <QStringList>
#include <QMap>
#include <QList>

#include "photo.h"

namespace PhotoStage
{
class PhotoWorkUnit
{
    public:

        static PhotoWorkUnit* instance();

        void beginTransaction();
        void endTransaction();

        void setRating(const QList<Photo>& list, int rating);
        void setFlag(const QList<Photo>& list, Photo::Flag flag);
        void setColorLabel(const QList<Photo>& list,
            Photo::ColorLabel color);

        void insertKeywords(const QStringList& words);
        void assignKeywords(const QStringList& words,
            const QList<Photo>& list);
        void removeKeywordsExcept(const QStringList& words,
            const QList<Photo>& list);

        QMap<QString, int>getPhotoKeywordsCount(const QList<Photo>& list) const;
        QStringList getPhotoKeywords(const Photo& photo) const;

        // Photo Items are created here.
        // ownership is transferred to the caller who should call delete on the objects
        QList<Photo> getPhotosByPath(long long path_id,
            bool includeSubDirs = true);

        QList<Photo> getPhotosById(QList<long long> idList);

        void updateExifInfo(const Photo& photo) const;    
        void regenerateHash(Photo &p);

protected:

        PhotoWorkUnit();

    private:

        static PhotoWorkUnit* mInstance;
        long long rebuildTree(long long parent_id, long long left);
};
}
#endif // PHOTOWORKUNIT_H
