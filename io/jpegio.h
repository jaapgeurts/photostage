#ifndef PHOTOSTAGE_JPEGIO_H
#define PHOTOSTAGE_JPEGIO_H

#include <QString>
#include <QImage>

#include "image.h"
#include "import/exivfacade.h"

namespace PhotoStage
{
class JpegIO
{
    public:

        JpegIO(const QByteArray& memFile, const ExifInfo& ex_info);
        JpegIO(const QString& filename, const ExifInfo& ex_info);

        const Image&      image() const;
        const QByteArray& colorProfile() const;

    private:

        Image      mImage;
        QByteArray mProfile;

        JpegIO();
        void initFromArray(const QByteArray& memFile, const ExifInfo& ex_info);
};
}
#endif // PHOTOSTAGE_JPEGIO_H
