#ifndef PHOTOSTAGE_PHOTO_H
#define PHOTOSTAGE_PHOTO_H

#include <QString>
#include <QMetaType>
#include <QSqlQuery>
#include <QSharedPointer>

#include "photoowner.h"
#include "import/exivfacade.h"
#include "image.h"
#include "engine/developrawparameters.h"

namespace PhotoStage
{
class PhotoModel;

class Photo
{
    //  class PhotoPrivate;

    public:

        enum Roles
        {
            DataRole     = Qt::UserRole + 100,
            DateTimeRole = Qt::UserRole + 101,
            FilenameRole = Qt::UserRole + 102
        };

        enum ColorLabel
        {
            LabelNoColor = 0,
            LabelRed     = 1,
            LabelGreen   = 2,
            LabelBlue    = 3,
            LabelYellow  = 4,
            LabelPurple  = 5,
            LabelOrange  = 6
        };

        enum Flag
        {
            FlagNone   = 0,
            FlagPick   = 1,
            FlagReject = 2
        };

        Photo();
        Photo(PhotoOwner* owner, const QImage& image, const QString& filename, long long id);
        Photo(QSqlQuery& query);

        long long id() const;

        // getters & setters
        void setOriginalImage(const Image& image);
        const Image& originalImage();
        void setLibraryPreview(const QImage& image);
        void setLibraryPreviewsRGB(const QImage& image);
        void setDevelopPreviewsRGB(const QImage& image);
        const QImage& libraryPreview();
        const QImage& libraryPreviewsRGB();
        const QImage& developPreviewsRGB();

        void setSrcImagePath(const QString& path);
        const QString& srcImagePath() const;

        void setExifInfo(const ExifInfo& exifInfo);
        ExifInfo&       exifInfo();
        const ExifInfo& exifInfo() const;

        void setRating(int rating);
        int rating() const;
        void setColorLabel(ColorLabel label);
        ColorLabel colorLabel() const;
        void setFlag(Flag flag);
        Flag flag() const;

        bool isRaw() const;
        void setIsRaw(bool isRaw);

        long long hash() const;
        void setHash(long long code);

        bool isNull() const;

        void setIsDownloadingPreview(bool value);
        bool isDownloadingPreview() const;

        void setKeywords(const QStringList& list);
        QStringList keywords() const;
        bool isDownloadingOriginal() const;
        void setIsDownloadingOriginal(bool value);

        void setOwner(PhotoOwner* owner);

    private:

        class PhotoPrivate
        {
            friend class Photo;

            public:

                // Constructors
                PhotoPrivate(PhotoOwner* owner, const QImage& image, const QString& filename, long long mId);
                PhotoPrivate(QSqlQuery& query);
                virtual ~PhotoPrivate();

            private:

                long long         mId;

                int               mRating;
                Photo::ColorLabel mColorLabel;
                Photo::Flag       mFlag;
                QImage            mLibraryPreview;
                QImage            mLibraryPreviewsRGB;
                QImage            mDevelopPreviewsRGB;
                Image             mOriginal;
                QString           mSrcImagePath;
                ExifInfo          mExifInfo;
                PhotoOwner*       mOwner;
                bool              mIsDownloadingPreview;
                bool              mIsDownloadingOriginal;
                QStringList       mKeywords;
                long long         mHashCode;
                bool              mIsRaw;
        };

        QSharedPointer<PhotoPrivate> d;
};
}
Q_DECLARE_METATYPE(PhotoStage::Photo)

#endif // PHOTO
