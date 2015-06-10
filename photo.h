#ifndef PHOTO
#define PHOTO

#include <QImage>
#include <QString>
#include <QMetaType>
#include <QSqlQuery>

#include "engine/colortransform.h"

class Photo
{
    public:

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

        // Constructors
        Photo();
        Photo(const Photo& info);
        Photo(const QImage& image, const QString& filename, long long id);
        Photo(QSqlQuery & query);
        virtual ~Photo();

        // getters & setters
        void setLibraryPreview(const QImage& image);
        const QImage& libraryPreview();
        const QImage& libraryPreviewsRGB(const ColorTransform &colorTransform);

        void setSrcImagePath(const QString &path);
        const QString& srcImagePath();

        void setRating(int rating);
        int rating();

        void setColorLabel(ColorLabel label);
        ColorLabel colorLabel();

        void setFlag(Flag flag);
        Flag flag();

    public:

        long long id;

private:

        int            mRating;
        ColorLabel     mColorLabel;
        Flag           mFlag;
        QImage         mLibraryPreview;
        QImage         mLibraryPreviewsRGB;
        QString        mSrcImagePath;
};

Q_DECLARE_METATYPE(Photo)
Q_DECLARE_METATYPE(Photo*)

#endif // PHOTO
