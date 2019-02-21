#ifndef PHOTOSTAGE_COLORTRANSFORM_H
#define PHOTOSTAGE_COLORTRANSFORM_H

#include <QHash>
#include <QImage>

#include <lcms2.h>

#include "image.h"

#include "constants.h"

namespace PhotoStage {
const QString PREVIEW_COLOR_SPACE = "Adobe98";
const QString WORKING_COLOR_SPACE = "ProPhoto";

class ColorTransform
{
public:
  enum Format {
    FORMAT_FLOAT,
    FORMAT_RGB32, // 8bit channels (RGB channel + extra)
    FORMAT_GRAYSCALE8,
    FORMAT_RGB48, //  16 bit channels RRRR GGGG BBBB
    FORMAT_BGR48_PLANAR,
    FORMAT_RGB48_PLANAR,
  };

  // convenience functions that retain the result for future use
  static ColorTransform getTransform(const QString& iden, const QString& from,
                                     const QString& to,
                                     Format inFormat  = FORMAT_BGR48_PLANAR,
                                     Format outFormat = FORMAT_BGR48_PLANAR);
  static ColorTransform getTransform(const QByteArray& srcProfile,
                                     const QString&    to,
                                     Format inFormat  = FORMAT_BGR48_PLANAR,
                                     Format outFormat = FORMAT_BGR48_PLANAR);

  static QString getMonitorProfilePath();

  ColorTransform();
  ColorTransform(const QString& from, const QString& to,
                 Format inFormat  = FORMAT_FLOAT,
                 Format outFormat = FORMAT_FLOAT);
  ColorTransform(const QByteArray& srcProfile, const QString& to,
                 Format inFormat, Format outFormat);
  ColorTransform(const cmsHTRANSFORM& cmsTransform);
  ~ColorTransform();

  bool           isValid() const;
  const QString& profileName();

  Image  transformImage(const Image& inImage) const;
  QImage transformToQImage(const Image& inImage) const;
  QImage transformQImage(const QImage& inImage) const;
  Image  transformFromQImage(const QImage& inImage) const;

private:
  static QHash<QString, ColorTransform> mTransformCache;
  static QString                        mMonitorProfilePath;

  QSharedPointer<char> mHTransform;
  QString              mProfileName;
};
} // namespace PhotoStage
#endif // PHOTOSTAGE_COLORTRANSFORM_H
