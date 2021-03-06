#ifndef PHOTOSTAGE_UTILS_H
#define PHOTOSTAGE_UTILS_H

#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QRect>
#include <QSize>
#include <QString>

namespace PhotoStage {
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

template <typename T> void deleteArray(T buf[])
{
  delete[] buf;
}

bool is_block_device(const std::string& path);

} // namespace PhotoStage

#endif // PHOTOSTAGE_UTILS_H
