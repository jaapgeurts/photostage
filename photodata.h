#ifndef PHOTODATA_H
#define PHOTODATA_H

#include <QImage>
#include "photo.h"

namespace PhotoStage
{
class PhotoData
{
    public:

        // Constructors
        PhotoData();
        PhotoData(const Photo& info);
        PhotoData(const QImage& image, const QString& filename, long long mId);
        PhotoData(QSqlQuery& query);
        virtual ~PhotoData();

        // getters & setters
        void setOriginal(const QImage& image);
        const QImage& original() const;
        void setLibraryPreview(const QImage& image);
        const QImage& libraryPreview() const;
        const QImage& libraryPreviewsRGB();

        void setSrcImagePath(const QString& path);
        const QString& srcImagePath() const;

        void setExifInfo(const ExifInfo& exifInfo);
        const ExifInfo& exifInfo() const;

        void setRating(int rating);
        int rating() const;

        void setColorLabel(Photo::ColorLabel label);
        Photo::ColorLabel colorLabel() const;

        void setFlag(Photo::Flag flag);
        Photo::Flag flag() const;

        long long id() const;

    private:

        long long         mId;

        int               mRating;
        Photo::ColorLabel mColorLabel;
        Photo::Flag       mFlag;
        QImage            mLibraryPreview;
        QImage            mLibraryPreviewsRGB;
        QImage            mOriginal;
        QString           mSrcImagePath;
        ExifInfo          mExifInfo;
};
}

#endif // PHOTODATA_H
