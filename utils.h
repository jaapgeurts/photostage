#ifndef PHOTOSTAGE_UTILS_H
#define PHOTOSTAGE_UTILS_H

#include <QRect>
#include <QSize>
#include <QByteArray>
#include <QString>
#include <QFileInfo>
#include <QFile>

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

long long computeImageFileHash(QFile& file);
long long computeImageFileHash(const QFileInfo& info);
long long computeImageFileHash(const QString& path);
}

#endif // PHOTOSTAGE_UTILS_H
