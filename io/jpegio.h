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

        static Image fromFile(const QByteArray& memFile, const ExifInfo &ex_info);
        static Image fromFile(const QString& filename, const ExifInfo &ex_info);
        static bool saveToFile(const Image& image, const QString& filename);

        JpegIO();
};
}
#endif // PHOTOSTAGE_JPEGIO_H
