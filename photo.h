#ifndef PHOTO
#define PHOTO

#include <QString>
#include <QMetaType>
#include <QSqlQuery>

#include "image.h"

#include "engine/colortransform.h"

namespace PhotoStage
{
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
            Photo(QSqlQuery& query);
            virtual ~Photo();

            // getters & setters
            void setOriginal(const QImage& image);
            const QImage& original() const;
            void setLibraryPreview(const QImage& image);
            const QImage& libraryPreview();
            const QImage& libraryPreviewsRGB();

            void setSrcImagePath(const QString& path);
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

            int        mRating;
            ColorLabel mColorLabel;
            Flag       mFlag;
            QImage     mLibraryPreview;
            QImage     mLibraryPreviewsRGB;
            QImage     mOriginal;
            QString    mSrcImagePath;
    };
}
Q_DECLARE_METATYPE(PhotoStage::Photo)
Q_DECLARE_METATYPE(PhotoStage::Photo*)

#endif // PHOTO