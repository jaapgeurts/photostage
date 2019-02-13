#include "utils.h"
#include "constants.h"
#if defined(Q_OS_UNIX)
#include <xxhash.h>
#elif defined(Q_OS_MAC)
#include "external/xxHash/xxhash.h"
#endif
namespace PhotoStage
{
/* fits the source frame into the destination frame and
 * centers the image */
QRect fitFrame(const QSize& src, const QSize& dst)
{
    int   ws = src.width();     // width source;
    int   hs = src.height();

    int   wd = dst.width();     // width destination
    int   hd = dst.height();

    float ratio = (float)ws / (float)hs;

    int   x  = 0;
    int   y  = 0;
    int   wn = wd;     // new width
    int   hn = hd;     // new height

    if (ratio > 1.0f)
    {
        hn = wd / ratio;
        y  = (hd - hn) / 2;
    }
    else
    {
        wn = hd * ratio;
        x  = (wd - wn) / 2;
    }
    return QRect(x, y, wn, hn);
}

long long computeImageFileHash(QFile& file)
{
    if (!file.isOpen())
        file.open(QIODevice::ReadOnly);
    QByteArray array = file.read(HASH_INPUT_LEN);
    return XXH64(array.constData(), array.length(), 0);
}

long long computeImageFileHash(const QFileInfo& info)
{
    QFile file(info.canonicalFilePath());

    return computeImageFileHash(file);
}

long long computeImageFileHash(const QString& path)
{
    QFile file(path);

    return computeImageFileHash(file);
}

}
// this code is needed for RawSpeed
#ifdef __cplusplus
extern "C" {
#endif
int rawspeed_get_number_of_processor_cores()
{
    return 1;
}
#ifdef __cplusplus
}
#endif
