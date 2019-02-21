#ifndef PHOTOSTAGE_PNGIO_H
#define PHOTOSTAGE_PNGIO_H

#include <QString>

#include "image.h"

namespace PhotoStage {
class PngIO
{
public:
  static Image fromFile(const QString& filename);
  static bool  saveToFile(const Image& image, const QString& filename);

  PngIO();
};
} // namespace PhotoStage
#endif // PHOTOSTAGE_PNGIO_H
