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
            void setRating(const QList<Photo> &list, int rating);
            void setFlag(const QList<Photo>& list, Photo::Flag flag);
            void setColorLabel(const QList<Photo>& list,
                Photo::ColorLabel color);

            void insertKeywords(const QStringList& words);
            void assignKeywords(const QStringList& words,
                const QList<Photo>& list);
            void removeKeywordsExcept(const QStringList& words,
                const QList<Photo>& list);

            QMap<QString,
            int> getPhotoKeywords(const QList<Photo>& list) const;

            // Photo Items are created here.
            // ownership is transferred to the caller who should call delete on the objects
            QList<Photo> getPhotosByPath(long long path_id,
                bool includeSubDirs = true);

            QList<Photo> getPhotosById(QList<long long> idList);

        protected:

            PhotoWorkUnit();

        private:

            static PhotoWorkUnit* mInstance;
    };
}
#endif // PHOTOWORKUNIT_H
