#ifndef LIBRARYMODULE
#define LIBRARYMODULE

#include <QWidget>

#include "photo.h"

namespace PhotoStage
{
class LibraryModule : public QWidget
{
    Q_OBJECT

    public:

        LibraryModule(QWidget* parent = 0);

        virtual void setPhotos(const QList<Photo>& list)
        {
            mPhotoInfoList = list;
        }

        virtual void setPhoto(const Photo& photo)
        {
            mPhoto = photo;
        }

        const QList<Photo>& photos() const
        {
            return mPhotoInfoList;
        }

    protected:

        QList<Photo> mPhotoInfoList;
        Photo        mPhoto;
};
}

#endif // LIBRARYMODULE
