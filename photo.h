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

    QImage image;
    QString fileName;
    long long id;

    void setRating(int rating);
    int rating();

    void setColorLabel(ColorLabel label);
    ColorLabel colorLabel();

    void setFlag(Flag flag);
    Flag flag();

private:
    int mRating;
    ColorLabel mColorLabel;
    Flag mFlag;

};

Q_DECLARE_METATYPE(Photo)
Q_DECLARE_METATYPE(Photo*)

#endif // PHOTO

