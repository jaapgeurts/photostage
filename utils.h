#ifndef PHOTOSTAGE_UTILS_H
#define PHOTOSTAGE_UTILS_H

#include <QRect>
#include <QSize>
#include <QByteArray>

namespace PhotoStage
{
/**
 * @brief Fits the source frame into the destination frame.
 *        Keeps aspect ratio and centers the frame
 * @param src
 * @param destFrame
 * @return
 */
QRect fitFrame(const QSize& src, const QSize& dst);

long long xxHash(const QByteArray& array);
}

#endif // PHOTOSTAGE_UTILS_H
