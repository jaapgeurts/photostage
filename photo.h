#ifndef PHOTO
#define PHOTO

#include <QImage>
#include <QString>
#include <QMetaType>
#include <QSqlQuery>

class Photo
{

public:

    enum ColorLabel { LabelNoColor =0, LabelRed =1, LabelGreen=2, LabelBlue=3, LabelYellow=4, LabelPurple=5, LabelOrange=6  };
    enum Flag {  FlagNone = 0, FlagPick = 1, FlagReject = 2 };

    Photo();
    Photo(const Photo& info);
    Photo(const QImage& image, const QString& filename, long long id);
    Photo(QSqlQuery & query);
    virtual ~Photo();

    void setLibraryPreview(const QImage& image);
    const QImage& libraryPreview();

    void setSrcImagePath(const QString &path);
    const QString& srcImagePath();

    void setPreviewCachePath(const QString &path);
    const QString& previewCachePath();

    void setRating(int rating);
    int rating();

    void setColorLabel(ColorLabel label);
    ColorLabel colorLabel();

    void setFlag(Flag flag);
    Flag flag();

    QImage rawImage() { return mRawImage; }
    void setRawImage(const QImage& image) { mRawImage = image; }

public:
    long long id;

private:
    int mRating;
    ColorLabel mColorLabel;
    Flag mFlag;
    QString mSrcImagePath;
    QString mLibraryPreviewPath;

    QImage mLibraryPreview;
    QImage mRawImage;


};

Q_DECLARE_METATYPE(Photo)
Q_DECLARE_METATYPE(Photo*)

#endif // PHOTO

