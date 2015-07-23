#ifndef PHOTOSTAGE_PHOTO_H
#define PHOTOSTAGE_PHOTO_H

#include <QString>
#include <QMetaType>
#include <QSqlQuery>
#include <QSharedPointer>

#include "import/exivfacade.h"

namespace PhotoStage
{
class PhotoData;
class PhotoModel;
class Photo
{
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
        Photo(PhotoModel* owner, const QImage& image,
            const QString& filename, long long id);
        Photo(QSqlQuery& query);

        long long id() const;

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

        void setColorLabel(ColorLabel label);
        ColorLabel colorLabel() const;

        void setFlag(Flag flag);
        Flag flag() const;

        bool isNull() const;

        void setIsDownloading(bool value);
        bool isDownloading() const;

        void setKeywords(const QStringList& list);
        QStringList keywords() const;

        void setOwner(PhotoModel* model);

    private:

        QSharedPointer<PhotoData> d;
};
}
Q_DECLARE_METATYPE(PhotoStage::Photo)

#endif // PHOTO
