#ifndef PHOTOSTAGE_JPEGIO_H
#define PHOTOSTAGE_JPEGIO_H

#include <QString>
#include <QImage>

namespace PhotoStage
{
class JpegIO
{
    public:

        static QImage readFile(const QByteArray &memFile, QByteArray &iccProfile);

        JpegIO();
};
}
#endif // PHOTOSTAGE_JPEGIO_H
