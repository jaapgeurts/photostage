#ifndef SQLPHOTOINFO
#define SQLPHOTOINFO

#include <QImage>
#include <QString>
#include <QMetaType>

class SqlPhotoInfo
{

public:

    enum ColorLabel { Red, Green, Blue, Yellow, Purple, Orange, NoColor };
    enum Flag { Rejected, Picked, NoFlag };

    SqlPhotoInfo();
    SqlPhotoInfo(const SqlPhotoInfo& info);
    SqlPhotoInfo(const QImage& image, const QString& filename, long long id);
    virtual ~SqlPhotoInfo();

    QImage image;
    QString fileName;
    long long id;

    void setRating(int rating);
    int getRating();

    void setColorLabel(ColorLabel label);
    ColorLabel getColorLabel();

    void setFlag(Flag flag);
    Flag getFlag();

private:
    int rating;
    ColorLabel color;
    Flag flag;

};

Q_DECLARE_METATYPE(SqlPhotoInfo)

#endif // SQLPHOTOINFO

