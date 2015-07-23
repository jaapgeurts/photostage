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
        PhotoData(PhotoModel* owner, const QImage& image,
            const QString& filename, long long mId);
        PhotoData(QSqlQuery& query);
        virtual ~PhotoData();

        // getters & setters
        void setOriginal(const QImage& image);
        const QImage& original() const;
        void setLibraryPreview(const QImage& image);
        void setLibraryPreviewsRGB(const QImage& image);
        const QImage& libraryPreview();
        const QImage& libraryPreviewsRGB();

        void setSrcImagePath(const QString& path);
        const QString& srcImagePath() const;

        void setExifInfo(const ExifInfo& exifInfo);
        ExifInfo&       exifInfo();
        const ExifInfo& exifInfo() const;

        void setRating(int rating);
        int rating() const;

        void setColorLabel(Photo::ColorLabel label);
        Photo::ColorLabel colorLabel() const;

        void setFlag(Photo::Flag flag);
        Photo::Flag flag() const;

        long long id() const;

        PhotoModel* owner() const;
        void setOwner(PhotoModel* owner);

        void setIsDownloading(bool value);
        bool isDownloading() const;

        void setKeywords(const QStringList& list);
        QStringList keywords() const;

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
        PhotoModel*       mOwner;
        bool              mIsDownloading;
        QStringList       mKeywords;
};
}

#endif // PHOTODATA_H
